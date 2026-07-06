#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "kinova_msgs/msg/pose_velocity.hpp"
#include <chrono>

class KinovaTeleop : public rclcpp::Node
{
public:
    KinovaTeleop() : Node("kinova_teleop")
    {
        this->latest_message = kinova_msgs::msg::PoseVelocity();
        this->kinova_vel_publisher = this->create_publisher<kinova_msgs::msg::PoseVelocity>("/j2n6s300_driver/in/cartesian_velocity", 10);

        auto handle_input = [this](geometry_msgs::msg::Twist::UniquePtr msg) -> void {
            this->latest_message.twist_linear_x = msg->linear.x;
            this->latest_message.twist_linear_y = msg->linear.y;
            this->latest_message.twist_linear_z = msg->linear.z;

            this->latest_message.twist_angular_x = msg->angular.x;
            this->latest_message.twist_angular_y = msg->angular.y;
            this->latest_message.twist_angular_z = msg->angular.z;
        };

        this->input_subscription = this->create_subscription<geometry_msgs::msg::Twist>("/spacenav/twist", 10, handle_input);

        // Kinova arm needs to be updated at 100 Hz, so we need to publish that frequently
        // https://github.com/RRL-ALeRT/kinova-ros2#:~:text=Note%20on%20publish,the%20requested%20velocity.
        this->timer = create_wall_timer(std::chrono::milliseconds(10), [this]() -> void {
            if (this->latest_message != kinova_msgs::msg::PoseVelocity()) {
                RCLCPP_INFO(this->get_logger(), "Publishing nonzero message");
                this->kinova_vel_publisher->publish(this->latest_message);
            }
        });
    }
private:
    kinova_msgs::msg::PoseVelocity latest_message;
    rclcpp::Publisher<kinova_msgs::msg::PoseVelocity>::SharedPtr kinova_vel_publisher;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr input_subscription;
    rclcpp::TimerBase::SharedPtr timer;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_unique<KinovaTeleop>());

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}