#include "rokubi_force_torque_publisher/rokubi_force_torque_publisher.hpp"

RokubiForceTorquePublisher::RokubiForceTorquePublisher() : rclcpp::Node("rokubi_force_torque_publisher")
{
    // Read in parameter values
    rcl_interfaces::msg::ParameterDescriptor publish_topic_description = rcl_interfaces::msg::ParameterDescriptor();
    publish_topic_description.description = "Topic to publish geometry_msgs/WrenchStamped messages from the force torque sensor on";
    this->publish_topic = this->declare_parameter<std::string>("publish_topic", "ft_data", publish_topic_description);
    
    RCLCPP_DEBUG(this->get_logger(), "Opening topic %s", this->publish_topic.c_str());
    
    this->ft_publisher = this->create_publisher<geometry_msgs::msg::WrenchStamped>(this->publish_topic, rclcpp::SensorDataQoS());
    
    RCLCPP_DEBUG(this->get_logger(), "Successfully opened topic %s for publishing", this->publish_topic.c_str());
    
    RCLCPP_INFO(this->get_logger(), "Attempting to open serial port");
    int serial_port = open("/dev/ttyUSB0", O_RDWR);
    
    if (serial_port < 0)
    {
        RCLCPP_ERROR(this->get_logger(), "Error %d from opening device: %s", errno, strerror(errno));
        if (errno == 13)
        {
            RCLCPP_ERROR(this->get_logger(), "Add the current user to the dialout group");
        }
        throw std::exception();
    }
    
    memset(&(this->tty), 0, sizeof(this->tty));
    
    
    RCLCPP_INFO(this->get_logger(), "Successfully opened serial port %d", serial_port);
    
    if(tcgetattr(serial_port, &(this->tty)) != 0)
    {
        RCLCPP_ERROR(this->get_logger(), "Error %i from tcgetattr: %s", errno, strerror(errno));
    }
    
    this->tty.c_cflag &= ~PARENB; // Disable parity
    this->tty.c_cflag &= ~CSTOPB; // 1 stop bit
    this->tty.c_cflag |= CS8; // 8 bits per byte
    this->tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    this->tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    
    this->tty.c_lflag &= ~ICANON; // Disable canonical mode
    this->tty.c_lflag &= ~ECHO; // Disable echo
    this->tty.c_lflag &= ~ECHOE; // Disable erasure
    this->tty.c_lflag &= ~ECHONL; // Disable new-line echo
    this->tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    this->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    this->tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    this->tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    this->tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    this->tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    this->tty.c_cc[VMIN] = 0;
    
    // Set in/out baud rate to be 460800
    cfsetispeed(&tty, B460800);
    cfsetospeed(&tty, B460800);
    
    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        RCLCPP_ERROR(this->get_logger(), "Error %d from tcsetattr: %s", errno, strerror(errno));
    }
    
    // Enable linux FTDI low latency mode
    ioctl(serial_port, TIOCGSERIAL, &(this->ser_info));
    this->ser_info.flags |= ASYNC_LOW_LATENCY;
    ioctl(serial_port, TIOCSSERIAL, &(this->ser_info));
    
    // Initialize the object
    this->ft_sensor = std::make_unique<BotaForceTorqueSensorComm>(serial_port);
    
    // Set timer to read the data every 100 Hz
    this->publish_timer = this->create_wall_timer(std::chrono::milliseconds(10), [this]() -> void {
        this->publish_ft_data();
    });
}

void RokubiForceTorquePublisher::publish_ft_data()
{
    geometry_msgs::msg::WrenchStamped msg;
    
    switch(ft_sensor->readFrame())
    {
    case BotaForceTorqueSensorComm::VALID_FRAME:
        if (ft_sensor->frame.data.status.val>0)
        {
            RCLCPP_WARN(this->get_logger(), "No valid forces:");
            RCLCPP_WARN(this->get_logger(), "\tapp_took_too_long: %d",ft_sensor->frame.data.status.app_took_too_long);
            RCLCPP_WARN(this->get_logger(), "\toverrange: %d",ft_sensor->frame.data.status.overrange);
            RCLCPP_WARN(this->get_logger(), "\tinvalid_measurements: %d",ft_sensor->frame.data.status.invalid_measurements);
            RCLCPP_WARN(this->get_logger(), "\traw_measurements: %d",ft_sensor->frame.data.status.raw_measurements);
        }
        else
        {
            // Valid data; publish it!
            msg.header.stamp = this->get_clock()->now();

            msg.wrench.force.x = ft_sensor->frame.data.forces[0];
            msg.wrench.force.y = ft_sensor->frame.data.forces[1];
            msg.wrench.force.z = ft_sensor->frame.data.forces[2];

            msg.wrench.torque.x = ft_sensor->frame.data.forces[3];
            msg.wrench.torque.y = ft_sensor->frame.data.forces[4];
            msg.wrench.torque.z = ft_sensor->frame.data.forces[5];

            ft_publisher->publish(msg);
        }
        break;
        
    case BotaForceTorqueSensorComm::NOT_VALID_FRAME:
        RCLCPP_WARN(this->get_logger(), "No valid frame: %d",ft_sensor->get_crc_count());
        break;
        
    case BotaForceTorqueSensorComm::NOT_ALLIGNED_FRAME:
        RCLCPP_WARN(this->get_logger(), "Lost sync, trying to reconnect");
        break;

    case BotaForceTorqueSensorComm::NO_FRAME:
        break;
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    try
    {
        rclcpp::spin(std::make_unique<RokubiForceTorquePublisher>());
    }
    catch(std::exception &e)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rokubi_force_torque_publisher"), "The node ran into an unrecoverable error. Exiting.");
        rclcpp::shutdown();
        return EXIT_FAILURE;
    }
    
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}