#include "rokubi_force_torque_publisher/rokubi_force_torque_publisher.hpp"

#include "tf2/exceptions.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2/LinearMath/Transform.hpp"
#include <vector>

RokubiForceTorquePublisher::RokubiForceTorquePublisher() : rclcpp::Node("rokubi_force_torque_publisher")
{
    // Read in parameter values
    rcl_interfaces::msg::ParameterDescriptor raw_publish_topic_description = rcl_interfaces::msg::ParameterDescriptor();
    raw_publish_topic_description.description = "Topic to publish raw geometry_msgs/WrenchStamped messages from the force torque sensor on";
    this->raw_publish_topic = this->declare_parameter<std::string>("raw_publish_topic", "raw_ft_data", raw_publish_topic_description);

    rcl_interfaces::msg::ParameterDescriptor calibrated_publish_topic_description = rcl_interfaces::msg::ParameterDescriptor();
    calibrated_publish_topic_description.description = "Topic to publish calibrated geometry_msgs/WrenchStamped messages from the force torque sensor on";
    this->calibrated_publish_topic = this->declare_parameter<std::string>("calibrated_publish_topic", "calibrated_ft_data", calibrated_publish_topic_description);

    rcl_interfaces::msg::ParameterDescriptor tf_prefix_description = rcl_interfaces::msg::ParameterDescriptor();
    tf_prefix_description.description = "TF prefix used for frame IDs";
    this->tf_prefix = this->declare_parameter<std::string>("tf_prefix", "", tf_prefix_description);
    if (*(this->tf_prefix.end() - 1) != '/' || (this->tf_prefix.size() > 1 && *(this->tf_prefix.begin()) == '/'))
    {
        RCLCPP_ERROR(this->get_logger(), "tf_prefix should end in a trailing slash and should not have a leading slash");
        throw std::exception();
    }

    rcl_interfaces::msg::ParameterDescriptor end_effector_mass_description = rcl_interfaces::msg::ParameterDescriptor();
    end_effector_mass_description.description = "Mass of the end effector in kilograms";
    this->end_effector_mass = this->declare_parameter<double>("end_effector_mass", 0.0, end_effector_mass_description);

    rcl_interfaces::msg::ParameterDescriptor center_of_mass_description = rcl_interfaces::msg::ParameterDescriptor();
    center_of_mass_description.description = "Center of mass offset of the end effector in the sensor frame";
    std::vector<double> center_of_mass_list = this->declare_parameter<std::vector<double>>("center_of_mass_vector", std::vector<double>{0.0, 0.0, 0.0}, center_of_mass_description);
    this->center_of_mass_vector = geometry_msgs::msg::Vector3();
    this->center_of_mass_vector.x = center_of_mass_list[0];
    this->center_of_mass_vector.y = center_of_mass_list[1];
    this->center_of_mass_vector.z = center_of_mass_list[2];

    rcl_interfaces::msg::ParameterDescriptor force_bias_description = rcl_interfaces::msg::ParameterDescriptor();
    force_bias_description.description = "Force bias to subtract from raw sensor readings";
    std::vector<double> force_bias_list = this->declare_parameter<std::vector<double>>("force_bias_vector", std::vector<double>{0.0, 0.0, 0.0}, force_bias_description);
    this->force_bias_vector = geometry_msgs::msg::Vector3();
    this->force_bias_vector.x = force_bias_list[0];
    this->force_bias_vector.y = force_bias_list[1];
    this->force_bias_vector.z = force_bias_list[2];

    rcl_interfaces::msg::ParameterDescriptor torque_bias_description = rcl_interfaces::msg::ParameterDescriptor();
    torque_bias_description.description = "Torque bias to subtract from raw sensor readings";
    std::vector<double> torque_bias_list = this->declare_parameter<std::vector<double>>("torque_bias_vector", std::vector<double>{0.0, 0.0, 0.0}, torque_bias_description);
    this->torque_bias_vector = geometry_msgs::msg::Vector3();
    this->torque_bias_vector.x = torque_bias_list[0];
    this->torque_bias_vector.y = torque_bias_list[1];
    this->torque_bias_vector.z = torque_bias_list[2];

    // Initialize tf listener and buffer
    this->tf_buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    this->tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
    
    RCLCPP_DEBUG(this->get_logger(), "Opening topic %s", this->raw_publish_topic.c_str());
    
    this->raw_ft_publisher = this->create_publisher<geometry_msgs::msg::WrenchStamped>(this->raw_publish_topic, rclcpp::SensorDataQoS());
    this->calibrated_ft_publisher = this->create_publisher<geometry_msgs::msg::WrenchStamped>(this->calibrated_publish_topic, rclcpp::SensorDataQoS());
    
    RCLCPP_DEBUG(this->get_logger(), "Successfully opened topic %s for publishing", this->raw_publish_topic.c_str());
    
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

    this->latest_raw_force_torque_data = geometry_msgs::msg::WrenchStamped();
    
    // Set timer to read the data at 100 Hz
    this->publish_timer = this->create_wall_timer(std::chrono::milliseconds(10), [this]() -> void {
        this->publish_raw_ft_data();
        this->publish_calibrated_ft_data();
    });
}

tf2::Vector3 RokubiForceTorquePublisher::get_gravity_in_ft_frame()
{
    try
    {
        geometry_msgs::msg::TransformStamped base_link_to_ft_frame_transform = tf_buffer->lookupTransform(
            this->tf_prefix + "j2n6s300_ft_robot_side_connector",
            this->tf_prefix + "base_link",
            tf2::TimePointZero
        );
        tf2::Transform tf2_base_link_to_ft_frame_transform;
        tf2::fromMsg(base_link_to_ft_frame_transform.transform, tf2_base_link_to_ft_frame_transform);

        tf2::Vector3 gravity_in_base_link_frame = tf2::Vector3();
        gravity_in_base_link_frame.setX(0.0);
        gravity_in_base_link_frame.setY(0.0);
        gravity_in_base_link_frame.setZ(-9.80665);

        tf2::Vector3 gravity_in_force_torque_frame = tf2_base_link_to_ft_frame_transform.getBasis() * gravity_in_base_link_frame;
        return gravity_in_force_torque_frame;
    }
    catch (const tf2::TransformException & ex)
    {
        RCLCPP_WARN(this->get_logger(), "Failed to get transform from %s to %s; calibrated force torque data will be wrong", (this->tf_prefix + "j2n6s300_ft_robot_side_connector").c_str(), (this->tf_prefix + "base_link").c_str());
        return tf2::Vector3();
    }
}

void RokubiForceTorquePublisher::publish_raw_ft_data()
{
    switch(ft_sensor->readFrame())
    {
        case BotaForceTorqueSensorComm::VALID_FRAME:
        {
            geometry_msgs::msg::WrenchStamped msg;

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
                
                this->latest_raw_force_torque_data = msg;
                raw_ft_publisher->publish(msg);
            }
            break;
        }
        case BotaForceTorqueSensorComm::NOT_VALID_FRAME:
        {
            RCLCPP_WARN(this->get_logger(), "No valid frame: %d",ft_sensor->get_crc_count());
            break;
        }
        case BotaForceTorqueSensorComm::NOT_ALLIGNED_FRAME:
        {
            RCLCPP_WARN(this->get_logger(), "Lost sync, trying to reconnect");
            break;
        }
        case BotaForceTorqueSensorComm::NO_FRAME:
        {
            break;
        }
    }
}

void RokubiForceTorquePublisher::publish_calibrated_ft_data()
{
    // Store the latest data in a variable in case it is changed halfway through
    geometry_msgs::msg::WrenchStamped cur_latest_raw_force_torque_data = this->latest_raw_force_torque_data;

    // The force torque data is rotated about the y-axis, so flip it
    cur_latest_raw_force_torque_data.wrench.force.x *= -1;
    cur_latest_raw_force_torque_data.wrench.force.z *= -1;

    cur_latest_raw_force_torque_data.wrench.torque.x *= -1;
    cur_latest_raw_force_torque_data.wrench.torque.z *= -1;

    tf2::Vector3 tf2_gravity_in_ft_frame = this->get_gravity_in_ft_frame();
    RCLCPP_DEBUG(this->get_logger(), "gravity_in_ft_frame: x=%f y=%f z=%f", tf2_gravity_in_ft_frame.x(), tf2_gravity_in_ft_frame.y(), tf2_gravity_in_ft_frame.z());
    RCLCPP_DEBUG(this->get_logger(), "end_effector_mass=%f force_bias=(%f,%f,%f) torque_bias=(%f,%f,%f)", this->end_effector_mass,
                 this->force_bias_vector.x, this->force_bias_vector.y, this->force_bias_vector.z,
                 this->torque_bias_vector.x, this->torque_bias_vector.y, this->torque_bias_vector.z);

    geometry_msgs::msg::WrenchStamped calibrated_force_torque_data = geometry_msgs::msg::WrenchStamped();
    calibrated_force_torque_data.header = cur_latest_raw_force_torque_data.header;

    // Offset the forces using the given data (F_calibrated = F_raw - (m * g + F_bias))
    calibrated_force_torque_data.wrench.force.x = cur_latest_raw_force_torque_data.wrench.force.x - (this->end_effector_mass * tf2_gravity_in_ft_frame.x() + this->force_bias_vector.x);
    calibrated_force_torque_data.wrench.force.y = cur_latest_raw_force_torque_data.wrench.force.y - (this->end_effector_mass * tf2_gravity_in_ft_frame.y() + this->force_bias_vector.y);
    calibrated_force_torque_data.wrench.force.z = cur_latest_raw_force_torque_data.wrench.force.z - (this->end_effector_mass * tf2_gravity_in_ft_frame.z() + this->force_bias_vector.z);

    // Offset the torques using the given data (T_calibrated = T_raw - (m * center_of_mass_vector {cross} g + T_bias))
    tf2::Vector3 tf2_center_of_mass_vector = tf2::Vector3();
    tf2::fromMsg(this->center_of_mass_vector, tf2_center_of_mass_vector);
    tf2::Vector3 center_of_mass_cross_gravity_vector = tf2_center_of_mass_vector.cross(tf2_gravity_in_ft_frame);
    calibrated_force_torque_data.wrench.torque.x = cur_latest_raw_force_torque_data.wrench.torque.x - (this->end_effector_mass * center_of_mass_cross_gravity_vector.x() + this->torque_bias_vector.x);
    calibrated_force_torque_data.wrench.torque.y = cur_latest_raw_force_torque_data.wrench.torque.y - (this->end_effector_mass * center_of_mass_cross_gravity_vector.y() + this->torque_bias_vector.y);
    calibrated_force_torque_data.wrench.torque.z = cur_latest_raw_force_torque_data.wrench.torque.z - (this->end_effector_mass * center_of_mass_cross_gravity_vector.z() + this->torque_bias_vector.z);

    RCLCPP_DEBUG(this->get_logger(), "raw_force (after axis flip): fx=%f fy=%f fz=%f", cur_latest_raw_force_torque_data.wrench.force.x, cur_latest_raw_force_torque_data.wrench.force.y, cur_latest_raw_force_torque_data.wrench.force.z);
    RCLCPP_DEBUG(this->get_logger(), "calibrated_force: fx=%f fy=%f fz=%f", calibrated_force_torque_data.wrench.force.x, calibrated_force_torque_data.wrench.force.y, calibrated_force_torque_data.wrench.force.z);

    // Publish it!
    calibrated_ft_publisher->publish(calibrated_force_torque_data);
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