#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"

class DingoTeleop : public rclcpp::Node
{
public:
    DingoTeleop() : Node("dingo_teleop")
    {
        this->latest_message = geometry_msgs::msg::TwistStamped();
        dingo_cmd_vel_publisher = this->create_publisher<geometry_msgs::msg::TwistStamped>("/oarbot_blue/dingo/cmd_vel", 1);
        auto handle_input = [this](geometry_msgs::msg::Twist::UniquePtr msg) -> void {
            latest_message.twist = *msg;
            latest_message.header.stamp = this->get_clock()->now();
            latest_message.header.frame_id = "world";
        };

        input_subscription = this->create_subscription<geometry_msgs::msg::Twist>("/spacenav/twist", 10, handle_input);

        this->timer = create_wall_timer(std::chrono::milliseconds(100), [this]() -> void {
            RCLCPP_INFO(this->get_logger(), "Publishing message");
            this->dingo_cmd_vel_publisher->publish(this->latest_message);
        });
    }
private:
    geometry_msgs::msg::TwistStamped latest_message;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr dingo_cmd_vel_publisher;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr input_subscription;
    rclcpp::TimerBase::SharedPtr timer;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DingoTeleop>());
    
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}