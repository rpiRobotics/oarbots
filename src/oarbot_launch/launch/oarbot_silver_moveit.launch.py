from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("oarbot_silver", package_name="oarbot_silver_moveit").to_moveit_configs()

    return LaunchDescription([
        Node(
            package="moveit_ros_move_group",
            executable="move_group",
            output="screen",
            namespace="oarbot_silver",
            parameters=[
                moveit_config.to_dict()
            ]
        )
    ])