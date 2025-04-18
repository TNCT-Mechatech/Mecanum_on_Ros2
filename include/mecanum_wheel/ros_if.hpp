#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include "std_msgs/msg/float32_multi_array.hpp"
#include "sensor_msgs/msg/joy.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders; // NOLINT

class MecanumMovementIf : public rclcpp::Node
{
public:
  MecanumMovementIf() : Node("mecanum_movement")
  {
    // joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/joint_states", 1);
//    twist_sub_ = this->create_subscription<geometry_msgs::msg::TwistStamped>("cmd_rate", 10, std::bind(&PanTiltRosIf::onTwistReceived, this, std::placeholders::_1));
    // timer_ = this->create_wall_timer(10ms, std::bind(&MecanumMovementIf::onTimer, this));
  }

protected:
  // virtual void onTimer() = 0;
  //  virtual void onTwistReceived(const geometry_msgs::msg::TwistStamped::SharedPtr msg) = 0;
  virtual void onTwistReceived(const sensor_msgs::msg::Joy::SharedPtr msg) = 0;

  // virtual void onFloat32MultiArrayReceived(const std_msgs::msg::Float32MultiArray::SharedPtr msg) = 0;

  // void publishJointState(const sensor_msgs::msg::JointState &msg)
  // {
  //   joint_state_pub_->publish(msg);
  // }

private:
  // rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_{nullptr};
  // rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr twist_sub_{nullptr};
  
  // rclcpp::Subscription<std_msgs::msg::Float32MultiArray>::SharedPtr f32_sub_{nullptr};
  // rclcpp::TimerBase::SharedPtr timer_{nullptr};
};