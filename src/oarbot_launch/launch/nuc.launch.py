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
                    "azure_kinect_bringup.launch.py"
                ])
            ),
            launch_arguments={
                "azure_kinect_namespace": "nuc",
                "frame_prefix": "nuc/",
                "fps": "5",
                "color_resolution": "1080P",
                "depth_mode": "NFOV_2X2BINNED",
                "point_cloud": "true",
                "rgb_point_cloud": "true"
            }.items()
        ),
        Node(
            package="aruco_pose_estimation",
            executable="aruco_node.py",
            name="aruco_pose_estimation",
            namespace="nuc",
            parameters=[{
                "marker_size": 0.36,
                "aruco_dictionary_id": "DICT_4X4_50",
                "image_topic": "/nuc/rgb/image_raw",
                "use_depth_input": True,
                "depth_image_topic": "/nuc/depth_to_rgb/image_raw",
                "camera_info_topic": "/nuc/rgb/camera_info",
                "camera_frame": "nuc/rgb_camera_link",
                "detected_markers_topic": "/aruco/markers",
                "markers_visualization_topic": "/aruco/poses",
                "output_image_topic": "/aruco/image"
            }]
        ),
        Node(
            package="oarbot_pose_publisher",
            executable="oarbot_pose_publisher",
            name="oarbot_pose_publisher",
            namespace="nuc",
            parameters=[{
                "aruco_markers_topic": "/aruco/markers",
                "kinect_imu_topic": "/nuc/imu",
                "aruco_tag_ids": [0, 4],
                "oarbot_namespaces": ["oarbot_blue/", "oarbot_silver/"],
            }]
        )
    ])
