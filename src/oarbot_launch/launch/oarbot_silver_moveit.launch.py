from launch import LaunchDescription
from launch.actions import GroupAction, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import PushRosNamespace
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    moveit_launch = PathJoinSubstitution([
        FindPackageShare("oarbot_silver_moveit"),
        "launch",
        "move_group.launch.py",
    ])

    return LaunchDescription([
        GroupAction([
            PushRosNamespace("oarbot_silver"),

            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(moveit_launch)
            ),
        ])
    ])