#ifndef ROKUBI_FORCE_TORQUE_PUBLISHER
#define ROKUBI_FORCE_TORQUE_PUBLISHER

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/wrench_stamped.hpp"
#include "rokubi_force_torque_publisher/BotaForceTorqueSensorComm.hpp"

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
    std::string publish_topic;
    rclcpp::Publisher<geometry_msgs::msg::WrenchStamped>::SharedPtr ft_publisher;
    rclcpp::TimerBase::SharedPtr publish_timer;

    std::unique_ptr<BotaForceTorqueSensorComm> ft_sensor;
    struct termios tty;
    struct serial_struct ser_info;

    void publish_ft_data();
};

#endif