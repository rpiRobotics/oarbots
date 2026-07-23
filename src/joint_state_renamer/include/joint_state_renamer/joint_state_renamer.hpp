#ifndef JOINT_STATE_RENAMER_HPP
#define JOINT_STATE_RENAMER_HPP

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

#include <string>

class JointStateRenamer : public rclcpp::Node
{
public:
    JointStateRenamer();
private:
    std::string joint_state_prefix;
    std::string input_topic;
    std::string output_topic;

    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr input_subscription;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr output_publisher;

    void input_subscription_callback(const sensor_msgs::msg::JointState::SharedPtr msg);
    void rename_joint_states(const sensor_msgs::msg::JointState::SharedPtr joint_states);
    void publish_joint_states(const sensor_msgs::msg::JointState::SharedPtr joint_states);
};


#endif