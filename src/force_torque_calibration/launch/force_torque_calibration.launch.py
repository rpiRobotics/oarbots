from launch import LaunchDescription, LaunchContext
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def verify_oarbot_namespace(context: LaunchContext, *args, **kwargs) -> list[Node]:
    oarbot_namespace_text = LaunchConfiguration("oarbot_namespace").perform(context)
    if oarbot_namespace_text[-1] != "/" or (len(oarbot_namespace_text) > 1 and oarbot_namespace_text[0] == "/"):
        raise ValueError("oarbot_namespace should contain no leading slash and should have a trailing slash")
    
    return []

def generate_launch_description() -> LaunchDescription:
    oarbot_namespace_string = "oarbot_namespace"
    oarbot_namespace_argument = DeclareLaunchArgument(oarbot_namespace_string ,description="Namespace of the OARBot to calibrate; should have no leading slash and should contain a trailing slash")
    joint_angles_csv_string = "joint_angles_csv"
    joint_angles_csv_argument = DeclareLaunchArgument(joint_angles_csv_string, description="Path to the csv file containing rows of 6 numbers as joint positions for calibrating the OARBot")


    return LaunchDescription([
        oarbot_namespace_argument,
        joint_angles_csv_argument,
        Node(
            package="force_torque_calibration",
            executable="force_torque_calibration",
            name="force_torque_calibration",
            namespace=LaunchConfiguration(oarbot_namespace_string),
            parameters=[{
                "joint_angles_csv": LaunchConfiguration(joint_angles_csv_string),
                "tf_prefix": LaunchConfiguration(oarbot_namespace_string)
            }]
        ),
        OpaqueFunction(function=verify_oarbot_namespace)
    ])