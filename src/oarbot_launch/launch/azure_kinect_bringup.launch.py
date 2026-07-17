from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, GroupAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration
from launch_ros.actions import PushROSNamespace
from launch_ros.substitutions import FindPackageShare

def generate_launch_description() -> LaunchDescription:
    azure_kinect_namespace_string = "azure_kinect_namespace"
    azure_kinect_namespace_argument = DeclareLaunchArgument(azure_kinect_namespace_string, description="Namespace all topics and nodes the Azure Kinect are published under")

    return LaunchDescription([
        azure_kinect_namespace_argument,
        GroupAction(
            actions=[
                PushROSNamespace(LaunchConfiguration(azure_kinect_namespace_string)),
                IncludeLaunchDescription(
                    PythonLaunchDescriptionSource(
                        PathJoinSubstitution([
                            FindPackageShare("azure_kinect_ros_driver"),
                            "launch",
                            "driver.launch.py"
                        ])
                    ),
                    launch_arguments={
                        "name": "oarbot_silver/kinova"
                    }.items()
                )
            ]
        ),
    ])