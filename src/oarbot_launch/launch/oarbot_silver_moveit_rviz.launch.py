from launch import LaunchDescription
from launch.actions import GroupAction, IncludeLaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node, PushRosNamespace
from launch_ros.substitutions import FindPackageShare
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description() -> LaunchDescription:
    moveit_config = MoveItConfigsBuilder("oarbot_silver", package_name="oarbot_silver_moveit").to_moveit_configs()
    return LaunchDescription([
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            namespace='oarbot_silver',
            parameters=[
                moveit_config.robot_description,
                moveit_config.robot_description_semantic,
                moveit_config.robot_description_kinematics,
                moveit_config.planning_pipelines,
                moveit_config.joint_limits,
                {
                    "default_planning_pipeline": "ompl",
                    "move_group_namespace": "/oarbot_silver/move_group",
                },
            ],
            remappings=[
                ('/tf', '/tf'),
                ('/tf_static', '/tf_static'),
            ],
        )
    ])
    # moveit_launch = PathJoinSubstitution([
    #     FindPackageShare("oarbot_silver_moveit"),
    #     "launch",
    #     "moveit_rviz.launch.py",
    # ])

    # return LaunchDescription([
    #     GroupAction([
    #         PushRosNamespace("oarbot_silver"),

    #         IncludeLaunchDescription(
    #             PythonLaunchDescriptionSource(moveit_launch)
    #         ),
    #     ])
    # ])