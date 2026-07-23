import os
from typing import cast
import xacro
from xml.dom.minidom import Document

from ament_index_python import get_package_share_directory
from launch import LaunchDescription, LaunchContext
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def check_namespace_trailing_forward_slash(context: LaunchContext, *args, **kwargs) -> None:
    oarbot_namespace_text = LaunchConfiguration("oarbot_namespace").perform(context)

    if (oarbot_namespace_text[-1] != "/"):
        raise ValueError("oarbot_namespace must end in a forward slash")


def generate_robot_state_publisher_node(context: LaunchContext, *args, **kwargs) -> list[Node]:
    robot_description_text = LaunchConfiguration("robot_description").perform(context)

    xacro_file = os.path.join(get_package_share_directory("oarbot_description"), "urdf", robot_description_text)
    urdf_file = cast(Document, xacro.process_file(xacro_file))
    robot_description = urdf_file.toprettyxml()
    
    return [
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            name="robot_state_publisher",
            namespace=LaunchConfiguration("oarbot_namespace"),
            parameters=[{
                "robot_description": robot_description
            }]
        ),
    ]

def generate_joint_state_publisher_node(context: LaunchContext, *args, **kwargs) -> list[Node]:
    namespace_text = LaunchConfiguration("oarbot_namespace").perform(context)

    return [
        Node(
            package="joint_state_publisher",
            executable="joint_state_publisher",
            name="joint_state_publisher",
            namespace=namespace_text + "no_prefix/",
            parameters=[{
                "source_list": [
                    "kinova/j2n6s300_driver/out/joint_state",
                    "dingo/platform/joint_states"
                    # TODO: Add more as this is built up
                ]
            }]
        )
    ]


def generate_launch_description() -> LaunchDescription:
    """
    For a whole, singular OARBot, this launch file launches a joint_state_publisher and robot_state_publisher that publish 
    coordinate frames to /tf and /tf_static as well as a robot description to a namespaced robot_description topic.
    """


    oarbot_namespace_string = "oarbot_namespace"
    oarbot_namespace_argument = DeclareLaunchArgument(oarbot_namespace_string, description="Namespace of the OARBot whose topics will be subscribed to; should end with a forward slash")

    robot_description_string = "robot_description"
    robot_description_argument = DeclareLaunchArgument(robot_description_string, description="Name of the robot description file placed under the oarbot_description/urdf directory")


    return LaunchDescription([
        oarbot_namespace_argument,
        robot_description_argument,
        Node(
            package="joint_state_renamer",
            executable="joint_state_renamer",
            name="joint_state_renamer",
            namespace=LaunchConfiguration(oarbot_namespace_string),
            parameters=[{
                "joint_state_prefix": LaunchConfiguration(oarbot_namespace_string),
                "input_topic": "no_prefix/joint_states",
                "output_topic": "joint_states"
            }]
        ),
        OpaqueFunction(function=check_namespace_trailing_forward_slash),
        OpaqueFunction(function=generate_robot_state_publisher_node),
        OpaqueFunction(function=generate_joint_state_publisher_node)
    ])