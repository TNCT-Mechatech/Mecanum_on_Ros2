#pragma once

#include <pigpio.h>

#include <array>
#include <iomanip>
#include <iostream>
#include <optional>
#include <memory>

#include "Mecanum_on_Ros2/include/mecanum_wheel/mecanum.hpp"
#include "PCA9685_RasPi/PCA9685_Raspi.hpp"
#include "types.hpp"

class MecanumPCA9685_GPIO
{
 private:
  std::shared_ptr<PCA9685_RasPi> pwm_pca9685_, dir_pca9685_;  // ここだけ持つ

  PWMTYPE pwm_control[4];
  DIRTYPE dir_control[4];

  // std::array<std::optional<PCA9685_RasPi>, 4> pwm_pcaControllers;
  // std::array<std::optional<PCA9685_RasPi>, 4> dir_pcaControllers;

  float pwm_info_[4];
  float dir_info_[4];
  float set_dir_info_[4];

  Mecanum mw;

 public:
  MecanumPCA9685_GPIO(PWMTYPE pwm_control_[4], DIRTYPE dir_control_[4], std::shared_ptr<PCA9685_RasPi> pwm_pca9685, std::shared_ptr<PCA9685_RasPi> dir_pca9685) : pwm_pca9685_(pwm_pca9685), dir_pca9685_(dir_pca9685)
  {
    for (int i = 0; i < 4; i++)
    {
      // pwm instance setting
      pwm_control[i] = pwm_control_[i];
      if (pwm_control[i].controlType == ControlType::RASPIGPIO)
      {
        gpioSetMode(pwm_control[i].channel,
                    PI_OUTPUT);  // GPIOピンを出力モードに設定
        gpioSetPWMfrequency(pwm_control[i].channel,
                            pwm_control[i].freq);  // 周波数を設定
      }
      else if (pwm_control[i].controlType == ControlType::PCA9685)
      {
      //   if (pwm_control[i].handle < 0)
      //     pwm_pcaControllers[i].emplace(OpenById{pwm_control[i].PCA9685Id, pwm_control[i].freq});
      //   else
          // pwm_pcaControllers[i].emplace(OpenByHandle{pwm_control[i].handle, pwm_control[i].freq});
      }
      else
        std::cout << "ERROR : NOT FOUND CONTROL TYPE. REBOOT PROGRAM AND CHECK "
                     "THE CONSTRACTOR."
                  << std::endl;

      // dir instance setting
      dir_control[i] = dir_control_[i];
      if (dir_control[i].controlType == ControlType::RASPIGPIO)
      {
        gpioSetMode(dir_control[i].channel,
                    PI_OUTPUT);  // GPIOピンを出力モードに設定
      }
      else if (dir_control[i].controlType == ControlType::PCA9685)
      {
        // if (dir_control[i].handle < 0)
        //   dir_pcaControllers[i].emplace(OpenById{dir_control[i].PCA9685Id, dir_control[i].freq});
        // else
        //   dir_pcaControllers[i].emplace(OpenByHandle{dir_control[i].handle, dir_control[i].freq});
      }
      else
        std::cout << "ERROR : NOT FOUND CONTROL TYPE. REBOOT PROGRAM AND CHECK "
                     "THE CONSTRACTOR."
                  << std::endl;
    }
    printf("start mecanum\n");
  }

  void pinWrite(float joy_x, float joy_y, float joy_turn)
  {
    mw.control(joy_x, joy_y, joy_turn);

    /*
    [0] --→ 左前　 (FrontLeft)  [FL]
    [1] --→ 右前　 (FrontRight) [FR]
    [2] --→ 左後ろ (RearLeft)   [RL]
    [3] --→ 右後ろ (RearRight)  [RR]
    */

    for (int i = 0; i < 4; ++i)
    {
      pwm_info_[i] = mw.getPwm(i);
      dir_info_[i] = mw.getDir(i);

      // PWM制御
      switch (pwm_control[i].controlType)
      {
        case ControlType::PCA9685:
          // if (pwm_pcaControllers[i])
          {
            pwm_pca9685_->setPwm(pwm_control[i].channel, mw.getPwm(i));
          }
          break;
        case ControlType::RASPIGPIO:
          PwmGpio(pwm_control[i].channel, mw.getPwm(i));
          break;
        default:
          break;
      }

      int direction = dir_control[i].dir_conf ? !mw.getDir(i) : mw.getDir(i);
      set_dir_info_[i] = direction;

      // DIR制御
      switch (dir_control[i].controlType)
      {
        case ControlType::PCA9685:
          // if (dir_pcaControllers[i])
          {
            dir_pca9685_->setDigital(dir_control[i].channel, direction);
          }
          break;
        case ControlType::RASPIGPIO:
          gpioWrite(dir_control[i].channel, direction);
          break;
        default:
          break;
      }
    }
  }

  void printConfigInfo()
  {
    std::cout << "----- PWM -----" << std::endl;
    for (int i = 0; i < 4; i++)
    {
      std::cout << "--" << i << "--" << std::endl;
      switch (pwm_control[i].controlType)
      {
        case ControlType::RASPIGPIO:
          std::cout << "controlType: " << "RASPIGPIO" << std::endl;
          break;

        case ControlType::PCA9685:
          std::cout << "controlType: " << "PCA9685" << std::endl;
          break;

        default:
          break;
      }
      std::cout << "PCA9685Id: " << pwm_control[i].PCA9685Id << std::endl;
      std::cout << "Channel: " << pwm_control[i].channel << std::endl;
      std::cout << "Frequency: " << pwm_control[i].freq << std::endl;
    }

    std::cout << "----- DIR -----" << std::endl;
    for (int i = 0; i < 4; i++)
    {
      std::cout << "--" << i << "--" << std::endl;
      switch (dir_control[i].controlType)
      {
        case ControlType::RASPIGPIO:
          std::cout << "controlType: " << "RASPIGPIO" << std::endl;
          break;

        case ControlType::PCA9685:
          std::cout << "controlType: " << "PCA9685" << std::endl;
          break;

        default:
          break;
      }
      std::cout << "PCA9685Id: " << dir_control[i].PCA9685Id << std::endl;
      std::cout << "Channel: " << dir_control[i].channel << std::endl;
      std::cout << "Frequency: " << dir_control[i].freq << std::endl;
      std::cout << "direction: " << dir_control[i].dir_conf << std::endl;
    }
  }
  void printControlInfo()
  {
    std::cout << std::left << std::setw(14) << "No." << std::setw(10) << "pwm" << std::setw(10) << "dir" << std::setw(10) << "set_dir" << std::endl;
    for (int i = 0; i < 4; i++)
    {
      std::cout << std::left << std::setw(14) << i << std::setw(10) << pwm_info_[i] << std::setw(10) << dir_info_[i] << std::setw(10) << set_dir_info_[i] << std::endl;
    }
  }

 private:
  void PwmGpio(int pinName, float unitInterval)
  {
    gpioPWM(pinName, static_cast<int>(255.0 * unitInterval));
  }
};