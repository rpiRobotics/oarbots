#!/bin/bash
set -e

# Source the global ROS2 installation
source /opt/ros/jazzy/setup.bash

# Use the correct DDS provider
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp 

# Source the ROS2 workspace and Python Virtual Environment
source ~/ros2_ws/install/local_setup.bash
source ~/ros2_ws/.venv/bin/activate

# Run your target launch file
ros2 launch oarbot_launch tablet_gui.launch.py