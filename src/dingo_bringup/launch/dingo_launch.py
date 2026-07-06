from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, GroupAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import SetRemap, SetParameter
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    dingo_name_string = "dingo_name"
    dingo_name_launch_arg = DeclareLaunchArgument(
        dingo_name_string,
        description="Used as frame_prefix when publishing frames across /tf to differentiate between multiple dingo robots. No default value. Must be unique."
    )
    dingo_name = LaunchConfiguration(dingo_name_string)

    platform_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            "/etc/clearpath/platform/launch/platform-service.launch.py"
        )
    )

    # This will set /<namespace>/tf and /<namespace>/tf_static to simply publish to /tf and /tf_static
    # It will also introduct frame_prefix to the tf frames that are published to avoid multi-robot collisions
    return LaunchDescription([
        GroupAction([
            SetRemap(src="tf", dst="/tf"),
            SetRemap(src="tf_static", dst="/tf_static"),
            SetParameter(name="frame_prefix", value=dingo_name),
            platform_launch,
        ])
    ])