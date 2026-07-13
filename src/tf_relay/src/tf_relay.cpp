#include "tf_relay/tf_relay.hpp"

TfRelay::TfRelay() : Node("tf_relay")
{
    // Delcare both parameters
    rcl_interfaces::msg::ParameterDescriptor robot_namespace_description = rcl_interfaces::msg::ParameterDescriptor{};
    robot_namespace_description.description = "Namespace used by the robot; should be unique";
    this->robot_namespace = declare_parameter<std::string>("robot_namespace", "", robot_namespace_description);

    rcl_interfaces::msg::ParameterDescriptor tf_frame_prefix_description = rcl_interfaces::msg::ParameterDescriptor{};
    tf_frame_prefix_description.description = "Prefix for all tf cordinate frames being published to /tf; should be unique";
    this->tf_frame_prefix = declare_parameter<std::string>("tf_frame_prefix", "", tf_frame_prefix_description);


    if (robot_namespace.empty())
    {
        throw std::runtime_error("Error launching tf_relay: You must provide a non-blank value to the robot_namespace parameter");
    }

    // If there is a leading or trailing forward slash on the namespace, delete it
    if (this->robot_namespace.front() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "robot_namespace argument has a leading forward slash. Removing for now, but it should not be included in future runs");
        this->robot_namespace.erase(this->robot_namespace.begin());
    }
    if (this->robot_namespace.back() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "robot_namespace argument has a trailing forward slash. Removing for now, but it should not be included in future runs");
        this->robot_namespace.pop_back();
    }


    // If there isn't a trailing forward slash at the end of the frame prefix, add it
    if (!this->tf_frame_prefix.empty() && this->tf_frame_prefix.back() != '/')
    {
        RCLCPP_WARN(this->get_logger(), "tf_frame_prefix argument does not have a trailing forward slash. Adding one for now, but it should be included in future runs");
        this->tf_frame_prefix += '/';
    }
    // If there is a leading forward slash at the front of the frame prefix, delete it
    if (this->tf_frame_prefix.size() > 1 && this->tf_frame_prefix.front() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "tf_frame_prefix argument has a leading forward slash. Removing for now, but it should not be included in future runs.");
        this->tf_frame_prefix.erase(this->tf_frame_prefix.begin());
    }


    // Create strings for the namespaced tf and tf_static topics
    std::string namespaced_tf_topic = "/" + this->robot_namespace + "/tf";
    std::string namespaced_tf_static_topic = "/" + this->robot_namespace + "/tf_static";

    // Initialize the publishers
    this->tf_publisher = this->create_publisher<tf2_msgs::msg::TFMessage>("/tf", rclcpp::QoS(100));
    this->tf_static_publisher = this->create_publisher<tf2_msgs::msg::TFMessage>("/tf_static", rclcpp::QoS(1).transient_local().reliable());

    // Initialize the subscribers
    this->namespaced_tf_subscription = this->create_subscription<tf2_msgs::msg::TFMessage>(namespaced_tf_topic, rclcpp::QoS(100), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) -> void { namespaced_tf_callback(msg); });
    this->namespaced_tf_static_subscription = this->create_subscription<tf2_msgs::msg::TFMessage>(namespaced_tf_static_topic, rclcpp::QoS(1).transient_local().reliable(), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) -> void { namespaced_tf_static_callback(msg); });

    RCLCPP_INFO(this->get_logger(), "Node initialized successfully; Relaying %s -> %s and %s -> %s with prefix \"%s\"", namespaced_tf_topic.c_str(), "/tf", namespaced_tf_static_topic.c_str(), "/tf_static", this->tf_frame_prefix.c_str());
}



std::string TfRelay::add_frame_prefix(const std::string &frame) const
{
    // If the incoming frame is empty or just a slash, simply return it; we shouldn't add our frame prefix to these
    if (frame.empty() || frame == "/")
    {
        return frame;
    }

    return this->tf_frame_prefix + frame;
}

void TfRelay::republish_incoming_message(const tf2_msgs::msg::TFMessage::SharedPtr msg, rclcpp::Publisher<tf2_msgs::msg::TFMessage>::SharedPtr publisher)
{
    tf2_msgs::msg::TFMessage output = tf2_msgs::msg::TFMessage();

    output.transforms.reserve(msg->transforms.size());

    for (geometry_msgs::msg::TransformStamped transform : msg->transforms)
    {
        transform.header.frame_id = this->add_frame_prefix(transform.header.frame_id);
        transform.child_frame_id = this->add_frame_prefix(transform.child_frame_id);

        output.transforms.push_back(std::move(transform));
    }

    publisher->publish(output);
}

void TfRelay::namespaced_tf_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg)
{
    republish_incoming_message(msg, this->tf_publisher);
}

void TfRelay::namespaced_tf_static_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg)
{
    republish_incoming_message(msg, this->tf_static_publisher);
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    try
    {
        rclcpp::spin(std::make_shared<TfRelay>());
    }
    catch(const std::exception &e)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Node returned an error: %s", e.what());

        rclcpp::shutdown();
        return EXIT_FAILURE;
    }

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}