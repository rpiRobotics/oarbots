#include "dingo_tf_relay/dingo_tf_relay.hpp"

DingoTfRelay::DingoTfRelay() : Node("dingo_tf_relay")
{
    // Delcare both arguments
    rcl_interfaces::msg::ParameterDescriptor dingo_namespace_description = rcl_interfaces::msg::ParameterDescriptor{};
    dingo_namespace_description.description = "Namespace used by the Clearpath Dingo robot";
    this->dingo_namespace = declare_parameter<std::string>("dingo_namespace", "", dingo_namespace_description, false);

    rcl_interfaces::msg::ParameterDescriptor dingo_frame_prefix_description = rcl_interfaces::msg::ParameterDescriptor{};
    dingo_frame_prefix_description.description = "prefix for all tf cordinate frames coming from the Clearpath Dingo robot";
    this->dingo_frame_prefix = declare_parameter<std::string>("dingo_frame_prefix", "", dingo_frame_prefix_description);


    if (dingo_namespace.empty())
    {
        throw std::runtime_error("Error launching dingo_tf_relay: You must provide a non-blank value to the dingo_namespace parameter");
    }

    // If there is a leading or trailing forward slash on the namespace, delete it
    if (this->dingo_namespace.front() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "dingo_namespace argument has a leading forward slash. Removing for now, but it should not be included in future runs");
        this->dingo_namespace.erase(this->dingo_namespace.begin());
    }
    if (this->dingo_namespace.back() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "dingo_namespace argument has a trailing forwrard shasl. Removing for now, but it should not be included in future runs");
        this->dingo_namespace.pop_back();
    }


    // If there isn't a trailing forward slash at the end of the frame prefix, add it
    if (!this->dingo_frame_prefix.empty() && this->dingo_frame_prefix.back() != '/')
    {
        RCLCPP_WARN(this->get_logger(), "dingo_frame_prefix argument does not have a trailing forward slash. Adding one for now, but it should be included in future runs");
        this->dingo_frame_prefix += '/';
    }
    // If there is a leading forward slash at the front of the frame prefix, delete it
    if (this->dingo_frame_prefix.size() > 1 && this->dingo_frame_prefix.front() == '/')
    {
        RCLCPP_WARN(this->get_logger(), "dingo_frame_prefix argument has a leading forward slash. Removing for now, but it should not be included in future runs.");
        this->dingo_frame_prefix.erase(this->dingo_frame_prefix.begin());
    }


    // Create strings for the namespaced tf and tf_static topics
    std::string namespaced_tf_topic = "/" + this->dingo_namespace + "/tf";
    std::string namespaced_tf_static_topic = "/" + this->dingo_namespace + "/tf_static";

    // Initialize the publishers
    this->dingo_tf_publisher = this->create_publisher<tf2_msgs::msg::TFMessage>("/tf", rclcpp::QoS(100));
    this->dingo_tf_static_publisher = this->create_publisher<tf2_msgs::msg::TFMessage>("/tf_static", rclcpp::QoS(1).transient_local().reliable());

    // Initialize the subscribers
    this->dingo_namespaced_tf_subscription = this->create_subscription<tf2_msgs::msg::TFMessage>(namespaced_tf_topic, rclcpp::QoS(100), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) -> void { namespaced_tf_callback(msg); });
    this->dingo_namespaced_tf_static_subscription = this->create_subscription<tf2_msgs::msg::TFMessage>(namespaced_tf_static_topic, rclcpp::QoS(1).transient_local().reliable(), [this](const tf2_msgs::msg::TFMessage::SharedPtr msg) -> void { namespaced_tf_static_callback(msg); });

    RCLCPP_INFO(this->get_logger(), "Node initialized successfully; Relaying %s -> %s and %s -> %s with prefix \"%s\"", namespaced_tf_topic.c_str(), "/tf", namespaced_tf_static_topic.c_str(), "/tf_static", this->dingo_frame_prefix.c_str());
}



std::string DingoTfRelay::add_frame_prefix(const std::string &frame) const
{
    // If the incoming frame is empty or just a slash, simply return it; we shouldn't add our frame prefix to these
    if (frame.empty() || frame == "/")
    {
        return frame;
    }

    return this->dingo_frame_prefix + frame;
}

void DingoTfRelay::republish_incoming_message(const tf2_msgs::msg::TFMessage::SharedPtr msg, rclcpp::Publisher<tf2_msgs::msg::TFMessage>::SharedPtr publisher)
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

void DingoTfRelay::namespaced_tf_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg)
{
    republish_incoming_message(msg, this->dingo_tf_publisher);
}

void DingoTfRelay::namespaced_tf_static_callback(const tf2_msgs::msg::TFMessage::SharedPtr msg)
{
    republish_incoming_message(msg, this->dingo_tf_static_publisher);
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    try
    {
        rclcpp::spin(std::make_shared<DingoTfRelay>());
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