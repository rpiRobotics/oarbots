#ifndef DINGO_TF_RELAY_CPP
#define DINGO_TF_RELAY_CPP

#include "rclcpp/rclcpp.hpp"
#include "tf2_msgs/msg/tf_message.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"

class TfRelay : public rclcpp::Node
{
public:
    TfRelay();
private:
    std::string robot_namespace;
    std::string tf_frame_prefix;

    rclcpp::Subscription<tf2_msgs::msg::TFMessage>::SharedPtr namespaced_tf_subscription;
    rclcpp::Subscription<tf2_msgs::msg::TFMessage>::SharedPtr namespaced_tf_static_subscription;

    rclcpp::Publisher<tf2_msgs::msg::TFMessage>::SharedPtr tf_publisher;
    rclcpp::Publisher<tf2_msgs::msg::TFMessage>::SharedPtr tf_static_publisher;

    std::string add_frame_prefix(const std::string &frame) const;
    void republish_incoming_message(const tf2_msgs::msg::TFMessage::SharedPtr msg, rclcpp::Publisher<tf2_msgs::msg::TFMessage>::SharedPtr publisher);

    void namespaced_tf_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg);
    void namespaced_tf_static_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg);
};

#endif