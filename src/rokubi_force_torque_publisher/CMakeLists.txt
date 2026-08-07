cmake_minimum_required(VERSION 3.8)
project(rokubi_force_torque_publisher)


find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(rokubi_force_torque_publisher
  src/rokubi_force_torque_publisher.cpp
  src/BotaForceTorqueSensorComm.cpp)
ament_target_dependencies(
  rokubi_force_torque_publisher
  rclcpp
  geometry_msgs
)

target_include_directories(rokubi_force_torque_publisher
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY
  include
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS
  rokubi_force_torque_publisher
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
