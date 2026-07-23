from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    joint_state_prefix_string = "joint_state_prefix"
    input_topic_string = "input_topic"
    output_topic_string = "output_topic"
    namespace_string = "namespace"

    joint_state_argument = DeclareLaunchArgument(joint_state_prefix_string, description="Prefix to append to all joint states")
    input_topic_argument = DeclareLaunchArgument(input_topic_string, description="Topic to take in the joint states from")
    output_topic_argument = DeclareLaunchArgument(output_topic_string, description="Topic to output the prefixed joint states to")
    namespace_argument = DeclareLaunchArgument(namespace_string, default_value="", description="Namespace for this node")

    return LaunchDescription([
        joint_state_argument,
        input_topic_argument,
        output_topic_argument,
        namespace_argument,
        Node(
            package="joint_state_renamer",
            executable="joint_state_renamer",
            namespace=LaunchConfiguration(namespace_string),
            parameters=[{
                joint_state_prefix_string: LaunchConfiguration(joint_state_prefix_string),
                input_topic_string: LaunchConfiguration(input_topic_string),
                output_topic_string: LaunchConfiguration(output_topic_string)
            }]
        )
    ])