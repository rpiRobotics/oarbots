import os
import yaml

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def yaml_to_dict(path_to_yaml: str):
    with open(path_to_yaml, "r") as f:
        return yaml.load(f, Loader=yaml.SafeLoader)

def generate_launch_description() -> LaunchDescription:
    """
    Launches the Kinova Jaco2 driver node under the given namespace
    """
    
    kinova_namespace_string = "kinova_namespace"
    kinova_namespace_argument = DeclareLaunchArgument(kinova_namespace_string, description="Namespace all topics and nodes the Kinova arm are published under")

    kinova_yaml_params = yaml_to_dict(os.path.join(get_package_share_directory("kinova_bringup"), "launch/config", "robot_parameters.yaml"))
    
    configurable_parameters = [
        {"name": "use_urdf",              "default": "true"},
        {"name": "kinova_robotType",      "default": "j2n6s300"},
        {"name": "kinova_robotSerial",    "default": "not_set"},
        {"name": "use_jaco_v1_fingers",   "default": True},
        {"name": "feedback_publish_rate", "default": 0.1},
        {"name": "tolerance",             "default": 2.0},
    ]

    return LaunchDescription([
            kinova_namespace_argument,
            Node(
                package="kinova_driver",
                executable="kinova_arm_driver",
                parameters=[dict([(param["name"], param["default"]) for param in configurable_parameters]), kinova_yaml_params],
                output="screen",
                namespace=LaunchConfiguration(kinova_namespace_string)
            )
    ])