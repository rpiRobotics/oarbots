#ifndef TABLET_ARDUINO_PUBLISHER_HPP
#define TABLET_ARDUINO_PUBLISHER_HPP

#include <chrono>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"

class ArduinoReadNode : public rclcpp::Node
{
public:
    ArduinoReadNode();        
    ~ArduinoReadNode();

private:
    void readSerial();
    bool openSerial();
    void closeSerial();
    void handleSerialData(uint8_t raw_byte);

    std::string deadman_topic_;
    std::string estop_topic_;
    std::string com_port_;
    int serial_fd_;
    
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr deadman_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr estop_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

#endif