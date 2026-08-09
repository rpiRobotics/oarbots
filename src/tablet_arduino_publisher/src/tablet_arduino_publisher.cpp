#include "tablet_arduino_publisher/tablet_arduino_publisher.hpp"

ArduinoReadNode::ArduinoReadNode(): Node("tablet_arduino_talker"), serial_fd_(-1)
{
    // Parameters (equivalent of rospy.get_param('~...'))
    deadman_topic_ = this->declare_parameter<std::string>("arduino_deadman_switch_topic", "");
    estop_topic_ = this->declare_parameter<std::string>("arduino_e_stop_topic", "");
    com_port_ = this->declare_parameter<std::string>("com_port", "");

    if (deadman_topic_.empty() || estop_topic_.empty() || com_port_.empty()) {
        RCLCPP_ERROR(
        this->get_logger(),
        "Required parameters missing: "
        "arduino_deadman_switch_topic='%s', arduino_e_stop_topic='%s', com_port='%s'",
        deadman_topic_.c_str(), estop_topic_.c_str(), com_port_.c_str());
    }

    deadman_pub_ = this->create_publisher<std_msgs::msg::Bool>(
        deadman_topic_, rclcpp::QoS(1));
    estop_pub_ = this->create_publisher<std_msgs::msg::Bool>(
        estop_topic_, rclcpp::QoS(1));

    // Mirrors the original 1s startup delay before the timer starts firing.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    timer_ = this->create_wall_timer(std::chrono::milliseconds(40), std::bind(&ArduinoReadNode::readSerial, this));
}

ArduinoReadNode::~ArduinoReadNode()
{
    closeSerial();
}

void ArduinoReadNode::readSerial()
{
    if (!rclcpp::ok()) {
        return;
    }

    if (serial_fd_ < 0) {
        RCLCPP_INFO(this->get_logger(), "Trying to reconnect to serial");
        if (!openSerial()) {
        RCLCPP_WARN(this->get_logger(), "Serial disconnected");
        return;
        }
        RCLCPP_INFO(this->get_logger(), "Connected to serial");
    }

    uint8_t byte = 0;
    ssize_t n = ::read(serial_fd_, &byte, 1);

    if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        // Real I/O error (e.g. device unplugged) -> disconnect and retry later.
        RCLCPP_WARN(this->get_logger(), "Disconnecting from serial");
        closeSerial();
        RCLCPP_WARN(this->get_logger(), "Serial disconnected: %s", std::strerror(errno));
        return;
    }

    if (n == 1) {
        handleSerialData(byte);
    }

    // Equivalent of ser.reset_input_buffer()
    tcflush(serial_fd_, TCIFLUSH);
}

bool ArduinoReadNode::openSerial()
{
    serial_fd_ = ::open(com_port_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_fd_ < 0) {
        RCLCPP_WARN(
        this->get_logger(), "Failed to open serial port %s: %s",
        com_port_.c_str(), std::strerror(errno));
        return false;
    }

    termios tty{};
    if (tcgetattr(serial_fd_, &tty) != 0) {
        RCLCPP_WARN(this->get_logger(), "tcgetattr failed: %s", std::strerror(errno));
        closeSerial();
        return false;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);   // enable receiver, ignore modem control lines
    tty.c_cflag &= ~PARENB;            // no parity
    tty.c_cflag &= ~CSTOPB;            // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                // 8 data bits
    tty.c_cflag &= ~CRTSCTS;           // no hardware flow control

    tty.c_lflag = 0;                   // raw input, no canonical mode
    tty.c_oflag = 0;                   // raw output
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // no software flow control
    tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Non-blocking reads; the 40 ms timer period provides the effective
    // "timeout" behaviour that pyserial's timeout=0.04 gave us.
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(serial_fd_, TCSANOW, &tty) != 0) {
        RCLCPP_WARN(this->get_logger(), "tcsetattr failed: %s", std::strerror(errno));
        closeSerial();
        return false;
    }

    tcflush(serial_fd_, TCIOFLUSH);
    return true;
}

void ArduinoReadNode::closeSerial()
{
    if (serial_fd_ >= 0) {
        ::close(serial_fd_);
        serial_fd_ = -1;
    }
}

// Equivalent of handle_serial_data(): expects a single ASCII digit byte.
void ArduinoReadNode::handleSerialData(uint8_t raw_byte)
{
    if (raw_byte < '0' || raw_byte > '9') {
        RCLCPP_WARN(this->get_logger(), "Garbage serial data");
        return;
    }
    int output = raw_byte - '0';

    std_msgs::msg::Bool deadman_msg;
    std_msgs::msg::Bool estop_msg;

    switch (output) {
        case 0:
        estop_msg.data = false;
        deadman_msg.data = true;
        break;
        case 1:
        estop_msg.data = false;
        deadman_msg.data = false;
        break;
        case 2:
        estop_msg.data = true;
        deadman_msg.data = true;
        break;
        case 3:
        estop_msg.data = true;
        deadman_msg.data = false;
        break;
        default:
        RCLCPP_WARN(this->get_logger(), "Garbage serial data");
        return;
    }

    deadman_pub_->publish(deadman_msg);
    estop_pub_->publish(estop_msg);
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ArduinoReadNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}