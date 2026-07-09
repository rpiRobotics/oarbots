cmake_minimum_required(VERSION 3.28)
project(kinova_teleop)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(kinova_msgs REQUIRED)

add_executable(kinova_teleop src/kinova_teleop.cpp)
ament_target_dependencies(
  kinova_teleop
  rclcpp
  geometry_msgs
  kinova_msgs
)

install(TARGETS
  kinova_teleop
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
