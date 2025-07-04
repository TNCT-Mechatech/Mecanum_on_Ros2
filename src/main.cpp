#include <cmath>
#include <iostream>

#include "mecanum.hpp"
#include "pigpio.h"
#include "rclcpp/rclcpp.hpp"
// #include "ros_if.hpp"

#include "MecanumPCA9685_GPIO.hpp"
#include "SignalControl.hpp"

#include "sensor_msgs/msg/joy.hpp"

class MecanumWheel : public rclcpp::Node, public MecanumPCA9685_GPIO {

public:
  MecanumWheel(void)
      : Node("mecanum_wheel"), MecanumPCA9685_GPIO(pwm_controls, dir_controls) {
    printf("start mecanum\n");

    twist_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "sp3/joy", 10,
        std::bind(&MecanumWheel::onTwistReceived, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Opening Node : complete.");
  }

  ~MecanumWheel() {
    RCLCPP_INFO(this->get_logger(), "Node is shutting down...");

    pinWrite(0.0, 0.0, 0.0); // 停止

    gpioTerminate(); // pigpioの終了
  }

private:
  void onTwistReceived(const sensor_msgs::msg::Joy::SharedPtr msg) {

    // joy receive
    float joy_x = msg->axes[0];
    float joy_y = msg->axes[1] * -1;
    float joy_turn = msg->axes[2];

    bool x_button = msg->buttons[2]; // 射出方向
    bool y_button = msg->buttons[3]; // 反射出方向

    pinWrite(joy_x, joy_y, joy_turn);

    /*
    [0] --→ 左前　 (FrontLeft)  [FL]
    [1] --→ 右前　 (FrontRight) [FR]
    [2] --→ 左後ろ (RearLeft)   [RL]
    [3] --→ 右後ろ (RearRight)  [RR]
    */

    RCLCPP_INFO(this->get_logger(), "%f, %f, %f\n", joy_x, joy_y, joy_turn);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr
      twist_sub_ /*{nullptr}*/;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);

  // pigpioライブラリの初期化
  if (gpioInitialise() < 0) {
    std::cerr << "pigpioの初期化に失敗しました。" << std::endl;
    return 1;
  }

  rclcpp::spin(std::make_shared<MecanumWheel>());

  rclcpp::shutdown();
  return 0;
}
