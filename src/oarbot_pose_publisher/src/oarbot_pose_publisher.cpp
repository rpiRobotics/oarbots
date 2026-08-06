#include "oarbot_pose_publisher/oarbot_pose_publisher.hpp"

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "tf2/LinearMath/Quaternion.hpp"
#include "tf2/exceptions.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

#include <cmath>

constexpr double imu_avg_weight = 0.999;
constexpr int max_imu_samples = 1000;

constexpr double world_to_camera_base_x_meters = 0;
constexpr double world_to_camera_base_y_meters = 0;
constexpr double world_to_camera_base_z_meters = 2.6;

constexpr const char *kinect_base_frame = "nuc/camera_base";


OarbotPosePublisher::OarbotPosePublisher() : Node("oarbot_pose_publisher")
{
    // Declare parameters
    rcl_interfaces::msg::ParameterDescriptor aruco_markers_topic_description = rcl_interfaces::msg::ParameterDescriptor();
    aruco_markers_topic_description.description = "Topic to listen for aruco marker poses";
    this->aruco_markers_topic = this->declare_parameter<std::string>("aruco_markers_topic", "", aruco_markers_topic_description);

    rcl_interfaces::msg::ParameterDescriptor kinect_imu_topic_description = rcl_interfaces::msg::ParameterDescriptor();
    kinect_imu_topic_description.description = "Topic to listen for Azure Kinect IMU data";
    this->kinect_imu_topic = this->declare_parameter<std::string>("kinect_imu_topic", "", kinect_imu_topic_description);

    rcl_interfaces::msg::ParameterDescriptor aruco_tag_ids_description = rcl_interfaces::msg::ParameterDescriptor();
    aruco_tag_ids_description.description = "List of OARBot ArUco tag ID's; should be in the same order as the oarbot_namespaces paramter";
    this->aruco_tag_ids = this->declare_parameter<std::vector<int64_t>>("aruco_tag_ids", std::vector<int64_t>{}, aruco_tag_ids_description);

    rcl_interfaces::msg::ParameterDescriptor oarbot_namespaces_description = rcl_interfaces::msg::ParameterDescriptor();
    oarbot_namespaces_description.description = "List of OARBOt namespaces; should include a trailing slash and be in the same order as aruco_tag_ids";
    this->oarbot_namespaces = this->declare_parameter<std::vector<std::string>>("oarbot_namespaces", std::vector<std::string>{}, oarbot_namespaces_description);

    // Check for trailing slashes
    for (const std::string &oarbot_namespace : this->oarbot_namespaces)
    {
        if (*(oarbot_namespace.end() - 1) != '/')
        {
            throw std::invalid_argument("All OARBot namespaces must end in a trailing slash");
        }
    }

    if (this->oarbot_namespaces.size() != this->aruco_tag_ids.size())
    {
        throw std::invalid_argument("aruco_tag_ids and oarbot_namespaces must be the same size list");
    }

    this->aruco_tag_data = std::unordered_map<int64_t, std::string>();
    for (int i = 0; i < this->aruco_tag_ids.size(); i++)
    {
        this->aruco_tag_data[this->aruco_tag_ids[i]] = this->oarbot_namespaces[i];
    }

    // Initialize both latest variables to default values
    this->aruco_marker_latest = aruco_interfaces::msg::ArucoMarkers();
    this->kinect_imu_average = sensor_msgs::msg::Imu();
    this->received_imu = false;


    // Initialize subscribers
    this->aruco_markers_subscription = this->create_subscription<aruco_interfaces::msg::ArucoMarkers>(this->aruco_markers_topic, rclcpp::QoS(10), [this](const aruco_interfaces::msg::ArucoMarkers::SharedPtr msg) -> void { aruco_markers_callback(msg); });
    this->kinect_imu_subscription = this->create_subscription<sensor_msgs::msg::Imu>(this->kinect_imu_topic, rclcpp::QoS(10), [this](const sensor_msgs::msg::Imu::SharedPtr msg) -> void { kinect_imu_callback(msg); });
    
    // Initialize tf listener, buffer, and broadcaster
    this->tf_buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    this->tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
    this->tf_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

    // Initialize the world to kinect base link transformation; time stamp will be added right before publishing (see this->publish_kinect_tf())
    this->cur_kinect_to_world_transform = geometry_msgs::msg::TransformStamped();
    this->cur_kinect_to_world_transform.header.frame_id = "world";
    this->cur_kinect_to_world_transform.child_frame_id = kinect_base_frame;

    // Every 60 seconds, reset the sample count to zero; this forces a re-calibration of the pose of the azure kinect from its IMU data
    this->imu_reset_timer = this->create_wall_timer(std::chrono::seconds(60), [this]() -> void {
        RCLCPP_DEBUG(this->get_logger(), "Re-sampling IMU data");
        this->imu_sample_count = 0;
    });

    // Every second, publish the tf data from the Kinect IMU
    // Even though it is updated every 60 seconds (see above), it should be published more frequently to be used by tf
    this->kinect_tf_timer = this->create_wall_timer(std::chrono::seconds(1), [this]() -> void {
        this->publish_kinect_tf();
    });
}

void OarbotPosePublisher::aruco_markers_callback(aruco_interfaces::msg::ArucoMarkers::SharedPtr msg)
{   
    // Set the latest aruco marker to this one
    this->aruco_marker_latest = *msg;

    // Assuming we have a valid imu stored (if we don't, it's not a big deal), call the tf publisher
    this->publish_oarbot_tf();
}

void OarbotPosePublisher::kinect_imu_callback(sensor_msgs::msg::Imu::SharedPtr msg)
{
    if (!this->received_imu)
    {
        this->kinect_imu_average = *msg;
        this->received_imu = true;
    }

    if (this->imu_sample_count > max_imu_samples)
    {
        return;
    }
    
    // Add to the exponential average of the imu data
    // Using an average helps smooth the data out
    this->kinect_imu_average.linear_acceleration.x = this->kinect_imu_average.linear_acceleration.x * imu_avg_weight + msg->linear_acceleration.x * (1.0 - imu_avg_weight);
    this->kinect_imu_average.linear_acceleration.y = this->kinect_imu_average.linear_acceleration.y * imu_avg_weight + msg->linear_acceleration.y * (1.0 - imu_avg_weight);
    this->kinect_imu_average.linear_acceleration.z = this->kinect_imu_average.linear_acceleration.z * imu_avg_weight + msg->linear_acceleration.z * (1.0 - imu_avg_weight);
    
    this->kinect_imu_average.angular_velocity.x = this->kinect_imu_average.angular_velocity.x * imu_avg_weight + msg->angular_velocity.x * (1.0 - imu_avg_weight);
    this->kinect_imu_average.angular_velocity.y = this->kinect_imu_average.angular_velocity.y * imu_avg_weight + msg->angular_velocity.y * (1.0 - imu_avg_weight);
    this->kinect_imu_average.angular_velocity.z = this->kinect_imu_average.angular_velocity.z * imu_avg_weight + msg->angular_velocity.z * (1.0 - imu_avg_weight);
    
    (this->imu_sample_count)++;

    // If we are done sampling, update the transformation
    if (this->imu_sample_count == max_imu_samples)
    {
        this->update_kinect_tf();
        return;
    }
}

void OarbotPosePublisher::update_kinect_tf()
{
    // Make local variables to prevent values being overwritten halfway through
    sensor_msgs::msg::Imu kinect_imu_latest = this->kinect_imu_average;

    this->cur_kinect_to_world_transform.transform.translation.x = world_to_camera_base_x_meters;
    this->cur_kinect_to_world_transform.transform.translation.y = world_to_camera_base_y_meters;
    this->cur_kinect_to_world_transform.transform.translation.z = world_to_camera_base_z_meters;

    // Figure out the rotation from the imu data
    // Assuming the only force on the Kinect is gravity, which will be pointing down in the world frame
    // Following this method: https://mwrona.com/posts/accel-roll-pitch/
    double gravity_norm = std::sqrt(std::pow(kinect_imu_latest.linear_acceleration.x, 2) + std::pow(kinect_imu_latest.linear_acceleration.y, 2) + std::pow(kinect_imu_latest.linear_acceleration.z, 2));
    double theta = std::asin(kinect_imu_latest.linear_acceleration.x / gravity_norm);
    double phi = std::atan(kinect_imu_latest.linear_acceleration.y / kinect_imu_latest.linear_acceleration.z);
    tf2::Quaternion kinect_quaternion;

    // Because we do not know the yaw, we assume the camera is facing forward
    kinect_quaternion.setRPY(phi, theta, 0);
    this->cur_kinect_to_world_transform.transform.rotation.x = kinect_quaternion.x();
    this->cur_kinect_to_world_transform.transform.rotation.y = kinect_quaternion.y();
    this->cur_kinect_to_world_transform.transform.rotation.z = kinect_quaternion.z();
    this->cur_kinect_to_world_transform.transform.rotation.w = kinect_quaternion.w();
}

void OarbotPosePublisher::publish_kinect_tf()
{
    // Update the transform timestamp
    this->cur_kinect_to_world_transform.header.stamp = this->get_clock()->now();
    
    // Send the transform!
    this->tf_broadcaster->sendTransform(this->cur_kinect_to_world_transform);
}

void OarbotPosePublisher::publish_oarbot_tf()
{
    // Make local variables to prevent values being overwritten halfway through
    aruco_interfaces::msg::ArucoMarkers cur_aruco_markers = this->aruco_marker_latest;

    for (std::pair<const int64_t, std::string> &cur_aruco_tag_data : this->aruco_tag_data)
    {
        try
        {   
            // Find the position of this tag in the list of returned ones via the topic, if it exists at all
            auto pos_iterator = std::find(cur_aruco_markers.marker_ids.begin(), cur_aruco_markers.marker_ids.end(), cur_aruco_tag_data.first);
            int position = pos_iterator - cur_aruco_markers.marker_ids.begin();
            
            // If we couldn't find the marker, don't try and transform to it; it is likely not visible on the camera
            if (position == cur_aruco_markers.marker_ids.size())
            {
                continue;
            }

            // Get the transform from the base_link to robot_arm_riser_aruco_mount_attach_link
            geometry_msgs::msg::TransformStamped cur_aruco_to_base_transform = tf_buffer->lookupTransform(cur_aruco_tag_data.second + "robot_arm_riser_aruco_mount_attach_link", cur_aruco_tag_data.second + "base_link", tf2::TimePointZero);
            
            // Start transforming from the rgb_camera_link to the tag
            geometry_msgs::msg::TransformStamped kinect_to_cur_aruco_transform;
            kinect_to_cur_aruco_transform.header.stamp = this->get_clock()->now();
            kinect_to_cur_aruco_transform.header.frame_id = "nuc/rgb_camera_link";
            kinect_to_cur_aruco_transform.child_frame_id = cur_aruco_tag_data.second + "robot_arm_riser_aruco_mount_attach_link";
    
            geometry_msgs::msg::Pose cur_aruco_tag_pose = cur_aruco_markers.poses[position];
            kinect_to_cur_aruco_transform.transform.translation.x = cur_aruco_tag_pose.position.x;
            kinect_to_cur_aruco_transform.transform.translation.y = cur_aruco_tag_pose.position.y;
            kinect_to_cur_aruco_transform.transform.translation.z = cur_aruco_tag_pose.position.z;
            kinect_to_cur_aruco_transform.transform.rotation.x = cur_aruco_tag_pose.orientation.x;
            kinect_to_cur_aruco_transform.transform.rotation.y = cur_aruco_tag_pose.orientation.y;
            kinect_to_cur_aruco_transform.transform.rotation.z = cur_aruco_tag_pose.orientation.z;
            kinect_to_cur_aruco_transform.transform.rotation.w = cur_aruco_tag_pose.orientation.w;
    
            // Combine both transforms
            tf2::Transform tf_cur_aruco_to_base, tf_kinect_to_cur_aruco;
            tf2::fromMsg(cur_aruco_to_base_transform.transform, tf_cur_aruco_to_base);
            tf2::fromMsg(kinect_to_cur_aruco_transform.transform, tf_kinect_to_cur_aruco);
            tf2::Transform tf_kinect_to_base = tf_kinect_to_cur_aruco * tf_cur_aruco_to_base;

            geometry_msgs::msg::TransformStamped kinect_to_base_transform;
            kinect_to_base_transform.header.stamp = this->get_clock()->now();
            kinect_to_base_transform.header.frame_id = "nuc/rgb_camera_link";
            kinect_to_base_transform.child_frame_id = cur_aruco_tag_data.second + "base_link";
            kinect_to_base_transform.transform = tf2::toMsg(tf_kinect_to_base);
            
            this->tf_broadcaster->sendTransform(kinect_to_base_transform);
        }
        catch (const tf2::TransformException &e)
        {
            // We likely didn't have transforms ready yet
            RCLCPP_WARN(this->get_logger(), "Waiting on transform from %s to %s", (cur_aruco_tag_data.second + "base_link").c_str(), (cur_aruco_tag_data.second + "robot_arm_riser_aruco_mount_attach_link").c_str());
        }
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<OarbotPosePublisher>());

    rclcpp::shutdown();
    return EXIT_SUCCESS;
}