from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    dingo_namespace_string = "dingo_namespace"
    dingo_frame_prefix_string = "dingo_frame_prefix"
    dingo_namespace_argument = DeclareLaunchArgument(dingo_namespace_string, description="Namespace used by the Clearpath Dingo robot. Should be unique to each Dingo")
    dingo_frame_prefix_argument = DeclareLaunchArgument(dingo_frame_prefix_string, description="Prefix for all tf cordinate frames coming from the Clearpath Dingo robot. Should be unique to each Dingo")

    return LaunchDescription([
        dingo_namespace_argument,
        dingo_frame_prefix_argument,
        Node(
            package="dingo_tf_relay",
            executable="dingo_tf_relay",
            namespace=LaunchConfiguration(dingo_namespace_string),
            parameters=[{dingo_namespace_string: LaunchConfiguration(dingo_namespace_string)}, {dingo_frame_prefix_string: LaunchConfiguration(dingo_frame_prefix_string)}]
        )
    ])