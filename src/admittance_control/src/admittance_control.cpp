#include "admittance_control/admittance_control.hpp"

#include "tf2/exceptions.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Transform.hpp"
#include "tf2/LinearMath/Vector3.h"

#include <algorithm>

// Min norm of the force that will be treated as a genuine force and will move the arm
constexpr double force_norm_cutoff = 5.0;
constexpr double torque_norm_cutoff = 0.35;
constexpr double max_linear_velocity = 3.0;
constexpr double max_angular_velocity = 3.0;
constexpr double estimated_end_effector_mass = 1.2;

constexpr double exponential_avg_scalar = 0.9;

AdmittanceControl::AdmittanceControl() : rclcpp::Node("admittance_control")
{
    rcl_interfaces::msg::ParameterDescriptor tf_prefix_description = rcl_interfaces::msg::ParameterDescriptor();
    tf_prefix_description.description = "TF prefix used for frame IDs";
    this->tf_prefix = this->declare_parameter<std::string>("tf_prefix", "", tf_prefix_description);
    if (*(this->tf_prefix.end() - 1) != '/' || (this->tf_prefix.size() > 1 && *(this->tf_prefix.begin()) == '/'))
    {
        RCLCPP_ERROR(this->get_logger(), "tf_prefix should end in a trailing slash and should not have a leading slash");
        throw std::exception();
    }

    this->publish_enable = this->declare_parameter<bool>("enable_initially", false);
    this->translation_enable = this->declare_parameter<bool>("enable_translation_initially", true);
    this->rotation_enable = this->declare_parameter<bool>("enable_rotation_initially", true);

    this->cur_linear_velocity = tf2::Vector3();
    this->cur_angular_velocity = tf2::Vector3();

    // Initialize subscriptions
    this->calibrated_ft_subscription = this->create_subscription<geometry_msgs::msg::WrenchStamped>("force_torque_calibrated", rclcpp::SensorDataQoS(), [this](geometry_msgs::msg::WrenchStamped::SharedPtr msg) -> void { this->force_torque_callback(msg); });
    this->kinova_velocity_publisher = this->create_publisher<kinova_msgs::msg::PoseVelocity>("kinova/j2n6s300_driver/in/cartesian_velocity", rclcpp::QoS(1));

    this->enable_subscription = this->create_subscription<std_msgs::msg::Bool>("admittance_enable", rclcpp::QoS(1), [this](std_msgs::msg::Bool::SharedPtr msg) -> void { this->publish_enable = msg->data; });
    this->enable_translation_subscription = this->create_subscription<std_msgs::msg::Bool>("admittance_translation_enable", rclcpp::QoS(1), [this](std_msgs::msg::Bool::SharedPtr msg) -> void { this->translation_enable = msg->data; });
    this->enable_rotation_subscription = this->create_subscription<std_msgs::msg::Bool>("admittance_rotation_enable", rclcpp::QoS(1), [this](std_msgs::msg::Bool::SharedPtr msg) -> void { this->rotation_enable = msg->data; });

    // Initialize tf listener and buffer
    this->tf_buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    this->tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
}

geometry_msgs::msg::Wrench::SharedPtr AdmittanceControl::convert_to_base_link_frame(const geometry_msgs::msg::Wrench &wrench_in_ft_sensor_frame)
{
    try
    {
        geometry_msgs::msg::TransformStamped ft_frame_to_base_link_transform = tf_buffer->lookupTransform(
            this->tf_prefix + "base_link",
            this->tf_prefix + "j2n6s300_ft_robot_side_connector",
            tf2::TimePointZero
        );
        
        tf2::Transform tf2_ft_frame_to_base_link_transform;
        tf2::fromMsg(ft_frame_to_base_link_transform.transform, tf2_ft_frame_to_base_link_transform);

        tf2::Vector3 force_in_ft_frame;
        tf2::Vector3 torque_in_ft_frame;
        tf2::fromMsg(wrench_in_ft_sensor_frame.force, force_in_ft_frame);
        tf2::fromMsg(wrench_in_ft_sensor_frame.torque, torque_in_ft_frame);

        geometry_msgs::msg::Wrench::SharedPtr wrench_in_base_frame = std::make_shared<geometry_msgs::msg::Wrench>();
        
        tf2::Vector3 force_in_base_frame = tf2_ft_frame_to_base_link_transform.getBasis() * force_in_ft_frame;
        tf2::Vector3 torque_in_base_frame = tf2_ft_frame_to_base_link_transform.getBasis() * torque_in_ft_frame;

        wrench_in_base_frame->force = tf2::toMsg(force_in_base_frame);
        wrench_in_base_frame->torque = tf2::toMsg(torque_in_base_frame);

        return wrench_in_base_frame;
    }
    catch (const tf2::TransformException & ex)
    {
        RCLCPP_WARN(this->get_logger(), "Failed to get transform from %s to %s; not sending velocity commands to the arm", (this->tf_prefix + "j2n6s300_ft_robot_side_connector").c_str(), (this->tf_prefix + "base_link").c_str());
        return std::make_shared<geometry_msgs::msg::Wrench>();
    }
}

void AdmittanceControl::force_torque_callback(geometry_msgs::msg::WrenchStamped::SharedPtr msg)
{
    if (publish_enable)
    {

        geometry_msgs::msg::Wrench::SharedPtr wrench_in_base_link_frame = this->convert_to_base_link_frame(msg->wrench);
    
        // If the norm of the force is small enough, then publish zero velocity
        tf2::Vector3 force_in_base_frame;
        tf2::fromMsg(wrench_in_base_link_frame->force, force_in_base_frame);
    
        // We use torque in the force-torque frame, due to how the kinova arm works
        tf2::Vector3 torque_in_ft_frame;
        tf2::fromMsg(msg->wrench.torque, torque_in_ft_frame);
    
        if (force_in_base_frame.length() >= force_norm_cutoff)
        {
            // Since velocity = force / (mass * time), we must sum this up and divide by the mass and time
            // force torque publishes at about 100 Hz
            this->cur_linear_velocity += force_in_base_frame / (estimated_end_effector_mass * 100.0);
            
            // Make sure they don't go above a max size
            if (this->cur_linear_velocity.length() > max_linear_velocity)
            {
                this->cur_linear_velocity = this->cur_linear_velocity.normalize() * max_linear_velocity;
            }
        }
        else
        {
            // Set the cur velocitiy to zero
            this->cur_linear_velocity.setZero();
        }
    
        if (torque_in_ft_frame.length() >= torque_norm_cutoff)
        {
            // Assume the inertia tensor is diag(1)
            this->cur_angular_velocity += torque_in_ft_frame / (estimated_end_effector_mass * 100);
    
            if (this->cur_angular_velocity.length() > max_angular_velocity)
            {
                this->cur_angular_velocity = this->cur_angular_velocity.normalize() * max_angular_velocity;
            }
        }
        else 
        {
            this->cur_angular_velocity.setZero();
        }
    
        // Convert and publish this data
        kinova_msgs::msg::PoseVelocity arm_velocity = kinova_msgs::msg::PoseVelocity();
        if (translation_enable)
        {
            arm_velocity.twist_linear_x = this->cur_linear_velocity.x();
            arm_velocity.twist_linear_y = this->cur_linear_velocity.y();
            arm_velocity.twist_linear_z = this->cur_linear_velocity.z();
        }
        if (rotation_enable)
        {
            arm_velocity.twist_angular_x = this->cur_angular_velocity.x();
            arm_velocity.twist_angular_y = this->cur_angular_velocity.y();
            arm_velocity.twist_angular_z = this->cur_angular_velocity.z();
        }
    
        if (this->tf_prefix == "oarbot_silver/")
        {
            // OARBot silver has x-y values negated for translation
            arm_velocity.twist_linear_x *= -1;
            arm_velocity.twist_linear_y *= -1;
    
            // Torque: z is negated and x-y is flipped
            arm_velocity.twist_angular_z *= -1;
            double tmp = arm_velocity.twist_angular_x;
            arm_velocity.twist_angular_x = arm_velocity.twist_angular_y;
            arm_velocity.twist_angular_y = tmp;
        }
        else if (this->tf_prefix == "oarbot_blue")
        {
            // OARBot blue has x-y values negated for rotation
            arm_velocity.twist_angular_x *= -1;
            arm_velocity.twist_angular_y *= -1;
        }
    
    
        this->kinova_velocity_publisher->publish(arm_velocity);
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    try
    {
        rclcpp::spin(std::make_unique<AdmittanceControl>());
    }
    catch(std::exception &e)
    {
        RCLCPP_ERROR(rclcpp::get_logger("admittance_control"), "The node ran into an unrecoverable error. Exiting.");
        rclcpp::shutdown();
        return EXIT_FAILURE;
    }
    
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}