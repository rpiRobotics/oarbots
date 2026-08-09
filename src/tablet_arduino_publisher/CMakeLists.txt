cmake_minimum_required(VERSION 3.28)
project(tablet_arduino_publisher)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(tablet_arduino_publisher src/tablet_arduino_publisher.cpp)
ament_target_dependencies(
  tablet_arduino_publisher
  rclcpp
  std_msgs
)

target_include_directories(tablet_arduino_publisher
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS
  tablet_arduino_publisher
  DESTINATION lib/${PROJECT_NAME}
)
 
install(DIRECTORY
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
