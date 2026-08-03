cmake_minimum_required(VERSION 3.28)
project(oarbot_pose_publisher)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(aruco_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

add_executable(oarbot_pose_publisher src/oarbot_pose_publisher.cpp)
ament_target_dependencies(
  oarbot_pose_publisher
  rclcpp
  aruco_interfaces
  std_msgs
  sensor_msgs
  geometry_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
)

target_include_directories(oarbot_pose_publisher
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY
  include
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS
  oarbot_pose_publisher
  DESTINATION lib/${PROJECT_NAME}
)

# install(DIRECTORY
#   launch
#   DESTINATION share/${PROJECT_NAME}
# )

ament_package()
