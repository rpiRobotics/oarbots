cmake_minimum_required(VERSION 3.28)
project(dingo_teleop)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(dingo_teleop src/dingo_teleop.cpp)
ament_target_dependencies(
  dingo_teleop
  rclcpp
  geometry_msgs
)

install(TARGETS
  dingo_teleop
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
