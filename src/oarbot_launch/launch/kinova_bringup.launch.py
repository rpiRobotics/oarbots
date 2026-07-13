from launch import LaunchDescription
from launch_ros.actions import PushROSNamespace
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration
from launch.actions import IncludeLaunchDescription, GroupAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description() -> LaunchDescription:
    kinova_namespace_string = "namespace"
    kinova_namespace_argument = DeclareLaunchArgument(kinova_namespace_string, description="Namespace used by the kinova robot; should be unique")

    frame_prefix_string = "frame_prefix"
    frame_prefix_argument = DeclareLaunchArgument(frame_prefix_string,description="Prefix for tf frames from the Kinova arm; should be unique and include a trailing forward slash if not empty")

    kinova_robot_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            PathJoinSubstitution([
                FindPackageShare("kinova_bringup"),
                "launch",
                "kinova_robot_launch.py"
            ])
        ),
        launch_arguments=[
            (frame_prefix_string, LaunchConfiguration(frame_prefix_string))
        ]
    )

    namespace_group = GroupAction(
        actions=[
            PushROSNamespace(LaunchConfiguration(kinova_namespace_string)),
            kinova_robot_launch
        ]
    )
    
    return LaunchDescription([
        kinova_namespace_argument,
        frame_prefix_argument,
        namespace_group
    ])