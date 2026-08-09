from launch import LaunchDescription
from launch.actions import  DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    return LaunchDescription([
        Node(
            package="tablet_gui",
            executable="tablet_gui",
            name="tablet_gui",
            output="screen",
            namespace="tablet_gui"
        )
    ])