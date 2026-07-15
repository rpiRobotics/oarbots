import os
from typing import cast
import xacro
from xml.dom.minidom import Document

from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description() -> LaunchDescription:
    """
    For a whole, singular OARBot, this launch file launches a joint_state_publisher and robot_state_publisher that publish 
    coordinate frames to /tf and /tf_static as well as a robot description to a namespaced robot_description node.
    """


    oarbot_namespace_string = "oarbot_namespace"
    oarbot_namespace_argument = DeclareLaunchArgument(oarbot_namespace_string, description="namespace of the OARBot whose topics will be subscribed to; should end with a forward slash")

    xacro_file = os.path.join(get_package_share_directory("oarbot_description"), "urdf", "oarbot_silver.urdf.xacro")
    urdf_file = cast(Document, xacro.process_file(xacro_file))
    robot_description = urdf_file.toprettyxml()


    return LaunchDescription([
        oarbot_namespace_argument,
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            name="robot_state_publisher",
            namespace=LaunchConfiguration(oarbot_namespace_string),
            parameters=[{
                "robot_description": robot_description,
                "frame_prefix": LaunchConfiguration(oarbot_namespace_string)
            }]
        ),
        Node(
            package="joint_state_publisher",
            executable="joint_state_publisher",
            name="joint_state_publisher",
            namespace=LaunchConfiguration(oarbot_namespace_string),
            parameters=[{
                "source_list": [
                    "kinova/j2n6s300_driver/out/joint_state",
                    "dingo/platform/joint_states"
                    # TODO: Add more as this is built up
                ]
            }]
        ),
    ])