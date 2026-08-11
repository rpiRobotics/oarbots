cmake_minimum_required(VERSION 3.28)
project(admittance_control)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(kinova_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

add_executable(admittance_control src/admittance_control.cpp)
ament_target_dependencies(
  admittance_control
  rclcpp
  kinova_msgs
  geometry_msgs
  std_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
)

target_include_directories(admittance_control
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY
  include
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS
  admittance_control
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
