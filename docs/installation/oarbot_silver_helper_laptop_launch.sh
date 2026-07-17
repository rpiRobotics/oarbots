#!/bin/bash

# Source the global ROS2 installation
source /opt/ros/jazzy/setup.bash

# Source your specific workspace setup file
source ~/ros2_ws/install/local_setup.bash

# Run your target launch file
ros2 launch oarbot_launch oarbot_silver_helper_laptop.launch.py