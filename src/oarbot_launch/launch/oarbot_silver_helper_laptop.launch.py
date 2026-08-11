from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

def generate_launch_description() -> LaunchDescription:
    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare("oarbot_launch"),
                    "launch",
                    "dingo_bridge.launch.py"
                ])
            ),
            launch_arguments={
                "oarbot_name": "oarbot_silver"
            }.items()
        ),
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
                    "azure_kinect_bringup.launch.py"
                ])
            ),
            launch_arguments={
                "azure_kinect_namespace": "oarbot_silver/azure_kinect",
                "frame_prefix": "oarbot_silver/",
                "fps": "15",
                "color_resolution": "1080P"
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
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare("oarbot_launch"),
                    "launch",
                    "oarbot_silver_moveit.launch.py"
                ])
            )
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([
                    FindPackageShare("oarbot_launch"),
                    "launch",
                    "kinova_action_servers.launch.py"
                ])
            ),
            launch_arguments={
                "kinova_namespace": "oarbot_silver/kinova/",
                "joint_prefix": "oarbot_silver"
            }.items()
        ),
        Node(
            executable="rokubi_force_torque_publisher",
            package="rokubi_force_torque_publisher",
            name="rokubi_force_torque_publisher",
            namespace="oarbot_silver",
            parameters=[{
                "tf_prefix": "oarbot_silver/",
                "end_effector_mass": 1.1936317,
                "center_of_mass_vector": [-0.03988221,-0.00383832,-0.0523659],
                "force_bias_vector": [-1.98321924,-5.31865946,1.63274343],
                "torque_bias_vector": [-0.02886351,-0.11731537,0.09439291]
            }]
        )
    ])