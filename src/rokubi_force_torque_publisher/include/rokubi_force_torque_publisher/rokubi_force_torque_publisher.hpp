#ifndef ROKUBI_FORCE_TORQUE_PUBLISHER
#define ROKUBI_FORCE_TORQUE_PUBLISHER

#include "rokubi_force_torque_publisher/BotaForceTorqueSensorComm.hpp"

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/wrench_stamped.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "tf2/LinearMath/Vector3.hpp"
#include "tf2_ros/buffer.hpp"
#include "tf2_ros/transform_listener.hpp"

#include <string>
#include <fcntl.h>      // Contains file controls like O_RDWR
#include <errno.h>      // Error integer and strerror() function
#include <termios.h>    // Contains POSIX terminal control definitions
#include <unistd.h>     // write(), read(), close()
#include <sys/ioctl.h>
#include <linux/serial.h>

class RokubiForceTorquePublisher : public rclcpp::Node
{
public:
    RokubiForceTorquePublisher();
private:
    std::string raw_publish_topic;
    std::string calibrated_publish_topic;
    std::string tf_prefix;
    double end_effector_mass;
    geometry_msgs::msg::Vector3 center_of_mass_vector;
    geometry_msgs::msg::Vector3 force_bias_vector;
    geometry_msgs::msg::Vector3 torque_bias_vector;

    geometry_msgs::msg::WrenchStamped latest_raw_force_torque_data;

    rclcpp::Publisher<geometry_msgs::msg::WrenchStamped>::SharedPtr raw_ft_publisher;
    rclcpp::Publisher<geometry_msgs::msg::WrenchStamped>::SharedPtr calibrated_ft_publisher;
    rclcpp::TimerBase::SharedPtr publish_timer;

    std::shared_ptr<tf2_ros::TransformListener> tf_listener;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer;

    std::unique_ptr<BotaForceTorqueSensorComm> ft_sensor;
    struct termios tty;
    struct serial_struct ser_info;
    
    tf2::Vector3 get_gravity_in_ft_frame();
    void publish_raw_ft_data();
    void publish_calibrated_ft_data();
};

#endif