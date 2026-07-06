from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package="kinova_teleop",
            namespace="kinova_teleop",
            executable="kinova_teleop"
        ),
        Node(
            package="spacenav",
            executable="spacenav_node"
        )
    ])