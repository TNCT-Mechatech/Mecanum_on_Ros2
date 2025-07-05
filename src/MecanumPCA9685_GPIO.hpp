#pragma once

#include <array>
#include <iostream>
#include <optional>

#include <pigpio.h>

#include "PCA9685_RasPi/PCA9685_Raspi.hpp"
#include "Mecanum_on_Ros2/include/mecanum_wheel/mecanum.hpp"
#include "types.hpp"

class MecanumPCA9685_GPIO {
private:
  CTYPE pwm_control[4];
  DIRTYPE dir_control[4];


  std::array<std::optional<PCA9685_RasPi>, 4> pwm_pcaControllers;
  std::array<std::optional<PCA9685_RasPi>, 4> dir_pcaControllers;

  Mecanum mw;

public:
  MecanumPCA9685_GPIO(CTYPE pwm_control_[4], DIRTYPE dir_control_[4]) {

    for (int i = 0; i < 4; i++) {
      // pwm instance setting
      pwm_control[i] = pwm_control_[i];
      if (pwm_control[i].controlType == ControlType::RASPIGPIO) {

        gpioSetMode(pwm_control[i].channel,
                    PI_OUTPUT); // GPIOピンを出力モードに設定
        gpioSetPWMfrequency(pwm_control[i].channel,
                            pwm_control[i].freq); // 周波数を設定
      } else if (pwm_control[i].controlType == ControlType::PCA9685) {
        pwm_pcaControllers[i].emplace(pwm_control[i].PCA9685Id,
                                      pwm_control[i].freq);
      } else
        std::cout << "ERROR : NOT FOUND CONTROL TYPE. REBOOT PROGRAM AND CHECK "
                     "THE CONSTRACTOR."
                  << std::endl;

      // dir instance setting
      dir_control[i] = dir_control_[i];
      if (dir_control[i].controlType == ControlType::RASPIGPIO) {

        gpioSetMode(dir_control[i].channel,
                    PI_OUTPUT); // GPIOピンを出力モードに設定
      } else if (dir_control[i].controlType == ControlType::PCA9685) {
        dir_pcaControllers[i].emplace(dir_control[i].PCA9685Id,
                                      dir_control[i].freq);
      } else
        std::cout << "ERROR : NOT FOUND CONTROL TYPE. REBOOT PROGRAM AND CHECK "
                     "THE CONSTRACTOR."
                  << std::endl;
    }
    printf("start mecanum\n");
  }

  void pinWrite(float joy_x, float joy_y, float joy_turn) {
    mw.control(joy_x, joy_y, joy_turn);

    /*
    [0] --→ 左前　 (FrontLeft)  [FL]
    [1] --→ 右前　 (FrontRight) [FR]
    [2] --→ 左後ろ (RearLeft)   [RL]
    [3] --→ 右後ろ (RearRight)  [RR]
    */

    for (int i = 0; i < 4; ++i) {

      if (pwm_control[i].controlType == ControlType::PCA9685 &&
          pwm_pcaControllers[i]) {
        pwm_pcaControllers[i]->setPwm(pwm_control[i].channel, mw.getPwm(i));
      } else if (pwm_control[i].controlType == ControlType::RASPIGPIO) {
        PwmGpio(pwm_control[i].channel, mw.getPwm(i));
      }

      int direction = (dir_control[i].dir_conf ? -1 : 1) * mw.getDir(i);

      if (dir_control[i].controlType == ControlType::PCA9685 &&
          dir_pcaControllers[i]) {
        dir_pcaControllers[i]->setPwm(dir_control[i].channel, direction);
      } else if (dir_control[i].controlType == ControlType::RASPIGPIO) {
        gpioWrite(dir_control[i].channel, mw.getDir(i));
      }
    }
  }

  void printControlInfo()
  {
    std::cout << "----- PWM -----" << std::endl;
    for(int i=0;i<4;i++)
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
    for(int i=0;i<4;i++)
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
private:
  void PwmGpio(int pinName, float unitInterval) {
    gpioPWM(pinName, static_cast<int>(255.0 * unitInterval));
  }
};