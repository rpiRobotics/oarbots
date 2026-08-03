#ifndef OARBOT_POSE_PUBLISHER
#define OARBOT_POSE_PUBLISHER

#include "rclcpp/rclcpp.hpp"
#include "aruco_interfaces/msg/aruco_markers.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "tf2_ros/static_transform_broadcaster.hpp"
#include "tf2_ros/transform_broadcaster.hpp"
#include "tf2_ros/transform_listener.hpp"
#include "tf2_ros/buffer.hpp"

#include <string>
#include <vector>

class OarbotPosePublisher : public rclcpp::Node
{
public:
    OarbotPosePublisher();
private:
    std::string aruco_markers_topic;
    std::string kinect_imu_topic;
    std::vector<int64_t> aruco_tag_ids;
    std::vector<std::string> oarbot_namespaces;
    std::unordered_map<int64_t, std::string> aruco_tag_data;

    bool received_imu;
    int imu_sample_count;

    rclcpp::TimerBase::SharedPtr imu_reset_timer;

    aruco_interfaces::msg::ArucoMarkers aruco_marker_latest;
    sensor_msgs::msg::Imu kinect_imu_average;

    rclcpp::Subscription<aruco_interfaces::msg::ArucoMarkers>::SharedPtr aruco_markers_subscription;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr kinect_imu_subscription;

    std::shared_ptr<tf2_ros::TransformListener> tf_listener;
    std::unique_ptr<tf2_ros::Buffer> tf_buffer;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster;

    void aruco_markers_callback(const aruco_interfaces::msg::ArucoMarkers::SharedPtr msg);
    void kinect_imu_callback(const sensor_msgs::msg::Imu::SharedPtr msg);

    void publish_to_tf();
};

#endif