#include <iostream>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "pigpio.h"
#include "mecanum.hpp"
// #include "ros_if.hpp"

#include "sensor_msgs/msg/joy.hpp"

class MecanumWheel : public rclcpp::Node,
                     public Mecanum
{
public:
  MecanumWheel(void) : Node("mecanum_wheel"),
                       Mecanum()
  {
    printf("start mecanum\n");

    twist_sub_ = this->create_subscription<sensor_msgs::msg::Joy>("sp3/joy", 10, std::bind(&MecanumWheel::onTwistReceived, this, std::placeholders::_1));

    // GPIOピンを出力モードに設定
    gpioSetMode(7, PI_OUTPUT);
    gpioSetMode(8, PI_OUTPUT);
    gpioSetMode(24, PI_OUTPUT);
    gpioSetMode(25, PI_OUTPUT);

    // PWM信号を開始
    gpioSetPWMfrequency(12, 20 * 1000); // 周波数を設定
    gpioSetPWMfrequency(18, 20 * 1000);
    gpioSetPWMfrequency(20, 20 * 1000);
    gpioSetPWMfrequency(21, 20 * 1000);

    RCLCPP_INFO(this->get_logger(), "Opening Node : complete.");
  }

  ~MecanumWheel()
  {
    RCLCPP_INFO(this->get_logger(), "Node is shutting down...");

    gpioWrite(25, 0.0);
    gpioWrite(7, 0.0);
    gpioWrite(8, 0.0);
    gpioWrite(24, 0.0);

    // デューティサイクルを設定
    gpioPWM(12, 255.0 * 0.0);
    gpioPWM(18, 255.0 * 0.0);
    gpioPWM(20, 255.0 * 0.0);
    gpioPWM(21, 255.0 * 0.0);

    gpioTerminate(); // pigpioの終了
  }

private:


  void PwmGpio(int pinName, float unitInterval)
  {
    gpioPWM(pinName, (int)(255.0 * unitInterval));
  }

  void onTwistReceived(const sensor_msgs::msg::Joy::SharedPtr msg)
  {

    // joy receive
    float joy_x = msg->axes[0];
    float joy_y = msg->axes[1] * -1;
    float joy_turn = msg->axes[2];

    bool x_button = msg->buttons[2]; // 射出方向
    bool y_button = msg->buttons[3]; // 反射出方向

    /*
    [0] --→ 左前　 (FrontLeft)  [FL]
    [1] --→ 右前　 (FrontRight) [FR]
    [2] --→ 左後ろ (RearLeft)   [RL]
    [3] --→ 右後ろ (RearRight)  [RR]
    */
    /*
    // GPIOピンを出力モードに設定
    gpioSetMode(7, PI_OUTPUT);
    gpioSetMode(8, PI_OUTPUT);
    gpioSetMode(24, PI_OUTPUT);
    gpioSetMode(25, PI_OUTPUT);

    // PWM信号を開始
    gpioSetPWMfrequency(12, 20 * 1000); // 周波数を設定
    gpioSetPWMfrequency(18, 20 * 1000);
    gpioSetPWMfrequency(20, 20 * 1000);
    gpioSetPWMfrequency(21, 20 * 1000);
    */

    control(joy_x, joy_y, joy_turn);

    PwmGpio(12, getPwm(0));
    PwmGpio(18, getPwm(1));
    PwmGpio(20, getPwm(2));
    PwmGpio(21, getPwm(3));

    gpioWrite(7, getDir(0));
    gpioWrite(8, getDir(1));
    gpioWrite(24, getDir(2));
    gpioWrite(25, getDir(3));

    RCLCPP_INFO(this->get_logger(), "%f, %f, %f\n", joy_x, joy_y, joy_turn);
  }


private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr twist_sub_ /*{nullptr}*/;

};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  // pigpioライブラリの初期化
  if (gpioInitialise() < 0)
  {
    std::cerr << "pigpioの初期化に失敗しました。" << std::endl;
    return 1;
  }

  rclcpp::spin(std::make_shared<MecanumWheel>());
  
  rclcpp::shutdown();
  return 0;
}
