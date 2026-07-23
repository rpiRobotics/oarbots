cmake_minimum_required(VERSION 3.28)
project(joint_state_renamer)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(joint_state_renamer src/joint_state_renamer.cpp)
ament_target_dependencies(
  joint_state_renamer
  rclcpp
  sensor_msgs
)

target_include_directories(joint_state_renamer
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY
  include
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS
  joint_state_renamer
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
