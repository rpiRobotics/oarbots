from launch import LaunchDescription
from launch.actions import  DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    kinova_namespace_string = "kinova_namespace"
    kinova_namespace_argument = DeclareLaunchArgument(kinova_namespace_string, description="Namespace the Kinova arm is under; all nodes and actions will also be placed under this namespace")

    return LaunchDescription([
        kinova_namespace_argument,
        Node(
            package="kinova_driver",
            executable="joint_trajectory_action_server",
            name="joint_trajectory_action_server",
            namespace=LaunchConfiguration(kinova_namespace_string),
            arguments=[
                "j2n6s300",
                "oarbot_silver"
            ]
        ),
        Node(
            package="kinova_driver",
            executable="gripper_command_action_server",
            name="gripper_command_action_server",
            namespace=LaunchConfiguration(kinova_namespace_string),
            arguments=[
                "j2n6s300",
                "oarbot_silver"
            ]
        )
    ])