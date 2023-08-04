#include <chrono>
#include <functional>
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/u_int8_multi_array.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

struct motor_control_msg{
    std::uint8_t id;
    float motor_1;
    float motor_2;
    float motor_3;
    float motor_4;
};

template <typename T>
std::vector<uint8_t> serialize(const T& data) {
    std::vector<uint8_t> bytes(sizeof(data));
    std::memcpy(bytes.data(), &data, sizeof(data));
    return bytes;
}

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("mc_test")
    {
      publisher_ = this->create_publisher<std_msgs::msg::UInt8MultiArray>("/serial_write", 10);
      subscription_ = this->create_subscription<std_msgs::msg::UInt8MultiArray>(
      "/serial_read", 10, std::bind(&MinimalPublisher::topic_callback, this, _1));
      timer_ = this->create_wall_timer(100ms, std::bind(&MinimalPublisher::timer_callback, this));
    }

  private:
    void timer_callback()
    {
      auto data = motor_control_msg();
      data.id = 0;
      data.motor_1 = 0.1;
      data.motor_2 = 0.2;
      data.motor_3 = 0.3;
      data.motor_4 = 0.4;


      auto msg = std_msgs::msg::UInt8MultiArray();
      msg.data = serialize(data);
      msg.data.insert(msg.data.begin(),'t');
      msg.data.insert(msg.data.begin(),'s');
      msg.data.push_back('e');
      msg.data.push_back('n');
      publisher_->publish(msg);

      RCLCPP_INFO(this->get_logger(), "pub");
    }

    void topic_callback(const std_msgs::msg::UInt8MultiArray msg)
    {
      RCLCPP_INFO(this->get_logger(), "data: %ld" , msg.data.size());
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::UInt8MultiArray>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::UInt8MultiArray>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}