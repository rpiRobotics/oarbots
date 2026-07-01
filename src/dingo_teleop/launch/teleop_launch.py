from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package="dingo_teleop",
            namespace="dingo_teleop",
            executable="dingo_teleop"
        ),
        Node(
            package="spacenav",
            executable="spacenav_node"
        )
    ])