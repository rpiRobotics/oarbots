from typing import cast

from launch import LaunchDescription, LaunchContext
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_domain_bridge_node(context: LaunchContext, *args, **kwargs) -> list[Node]:
    oarbot_name_text = LaunchConfiguration("oarbot_name").perform(context)
    config_file_location = PathJoinSubstitution([
        FindPackageShare("oarbot_launch"), "config", oarbot_name_text + "_dingo_bridge.yaml"
    ])

    return [
        Node(
            package="domain_bridge",
            executable="domain_bridge",
            name="domain_bridge",
            namespace=oarbot_name_text,
            arguments=[config_file_location]
        )
    ]


def generate_launch_description() -> LaunchDescription:
    oarbot_name_string = "oarbot_name"
    oarbot_name_argument = DeclareLaunchArgument(oarbot_name_string, description="Name of the oarbot; should be one of: oarbot_blue, oarbot_silver")

    return LaunchDescription([
        oarbot_name_argument,
        OpaqueFunction(function=generate_domain_bridge_node)
    ])