from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

def generate_launch_description() -> LaunchDescription:
    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare("oarbot_launch"),
                    "launch",
                    "kinova_bringup.launch.py"
                ])
            ),
            launch_arguments={
                "kinova_namespace": "oarbot_silver/kinova"
            }.items()
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare("oarbot_launch"),
                    "launch",
                    "oarbot_state_publisher.launch.py"
                ])
            ),
            launch_arguments={
                "oarbot_namespace": "oarbot_silver/",
                "robot_description": "oarbot_silver.urdf.xacro"
            }.items()
        )
    ])