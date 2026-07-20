#!/bin/bash
set -e

# In order for the Azure Kinect node to launch, a display needs to be connected. This can be done by SSH'ing
# with -X, but we will use the laptop's built-in display variable to launch the node
export DISPLAY=:1

# Source the global ROS2 installation
source /opt/ros/jazzy/setup.bash

# Source your specific workspace setup file
source ~/ros2_ws/install/local_setup.bash

# Run your target launch file
ros2 launch oarbot_launch oarbot_silver_helper_laptop.launch.py