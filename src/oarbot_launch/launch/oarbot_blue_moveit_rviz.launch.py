
from launch import LaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description() -> LaunchDescription:
    moveit_config = MoveItConfigsBuilder("oarbot_blue", package_name="oarbot_blue_moveit").to_moveit_configs()
    rviz_config = PathJoinSubstitution([
        FindPackageShare("oarbot_launch"), "config", "oarbot_blue_moveit.rviz",
    ])
    return LaunchDescription([
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            namespace='oarbot_blue',
            arguments=['-d', rviz_config],
            parameters=[
                moveit_config.robot_description,
                moveit_config.robot_description_semantic,
                moveit_config.robot_description_kinematics,
                moveit_config.planning_pipelines,
                moveit_config.joint_limits,
            ],
            remappings=[
                ('tf', '/tf'),
                ('tf_static', '/tf_static'),
            ],
        )
    ])