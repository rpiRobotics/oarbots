#ifndef ADMITTANCE_CONTROL_HPP
#define ADMITTANCE_CONTROL_HPP

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/wrench_stamped.hpp"
#include "geometry_msgs/msg/wrench.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "std_msgs/msg/bool.hpp"
#include "kinova_msgs/msg/pose_velocity.hpp"
#include "tf2_ros/buffer.hpp"
#include "tf2_ros/transform_listener.hpp"

#include <string>
#include <memory>

class AdmittanceControl : public rclcpp::Node
{
public:
    AdmittanceControl();
private:
    std::string tf_prefix;

    bool publish_enable;
    bool translation_enable;
    bool rotation_enable;

    tf2::Vector3 cur_linear_velocity;
    tf2::Vector3 cur_angular_velocity;

    
    rclcpp::Subscription<geometry_msgs::msg::WrenchStamped>::SharedPtr calibrated_ft_subscription;
    rclcpp::Publisher<kinova_msgs::msg::PoseVelocity>::SharedPtr kinova_velocity_publisher;

    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr enable_subscription;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr enable_translation_subscription;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr enable_rotation_subscription;

    std::shared_ptr<tf2_ros::TransformListener> tf_listener;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer;

    geometry_msgs::msg::Wrench::SharedPtr convert_to_base_link_frame(const geometry_msgs::msg::Wrench &wrench_in_ft_sensor_frame);    
    void force_torque_callback(geometry_msgs::msg::WrenchStamped::SharedPtr msg);
};

#endif