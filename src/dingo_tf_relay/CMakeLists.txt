cmake_minimum_required(VERSION 3.28)
project(dingo_tf_relay)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(dingo_tf_relay
  src/dingo_tf_relay.cpp
)

ament_target_dependencies(
  dingo_tf_relay
  rclcpp
  tf2_msgs
  geometry_msgs
)

target_include_directories(dingo_tf_relay
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY
  include
  DESTINATION include/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  dingo_tf_relay
  DESTINATION lib/${PROJECT_NAME}
)


ament_package()
