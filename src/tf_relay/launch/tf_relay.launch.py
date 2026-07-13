from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    robot_namespace_string = "robot_namespace"
    tf_frame_prefix_string = "tf_frame_prefix"
    robot_namespace_argument = DeclareLaunchArgument(robot_namespace_string, description="Namespace used by the robot; should be unique")
    tf_frame_prefix_argument = DeclareLaunchArgument(tf_frame_prefix_string, description="Prefix for all tf cordinate frames being published to /tf; should be unique")

    return LaunchDescription([
        robot_namespace_argument,
        tf_frame_prefix_argument,
        Node(
            package="tf_relay",
            executable="tf_relay",
            namespace=LaunchConfiguration(robot_namespace_string),
            parameters=[{robot_namespace_string: LaunchConfiguration(robot_namespace_string)}, {tf_frame_prefix_string: LaunchConfiguration(tf_frame_prefix_string)}]
        )
    ])