#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"

class DingoTeleop : public rclcpp::Node
{
public:
    DingoTeleop() : Node("dingo_teleop")
    {
        dingo_cmd_vel_publisher = this->create_publisher<geometry_msgs::msg::TwistStamped>("/dingo_three/dingo/cmd_vel", 10);
        auto handle_input = [this](geometry_msgs::msg::Twist::UniquePtr msg) -> void {
            RCLCPP_INFO(this->get_logger(), "Received twist with: (x = %lf, y = %lf, z = %lf), (a = %lf, b = %lf, c = %lf)", msg->linear.x, msg->linear.y, msg->linear.z, msg->angular.x, msg->angular.y, msg->angular.z);

            geometry_msgs::msg::TwistStamped output = geometry_msgs::msg::TwistStamped();
            output.twist = *msg;
            output.header.stamp = this->get_clock()->now();
            output.header.frame_id = "world";

            this->dingo_cmd_vel_publisher->publish(output);
            RCLCPP_INFO(this->get_logger(), "Successfully published message");
        };

        input_subscription = this->create_subscription<geometry_msgs::msg::Twist>("/spacenav/twist", 10, handle_input);
    }
private:
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr dingo_cmd_vel_publisher;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr input_subscription;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DingoTeleop>());
    
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}