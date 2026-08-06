#!/bin/bash
set -e

# In order for the Azure Kinect node to launch, a display needs to be connected. This can be done by SSH'ing
# with -X, but we will use the NUC's monitor
export DISPLAY=:0

# Source the global ROS2 installation
source /opt/ros/jazzy/setup.bash

# Use the correct DDS provider
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp 

# Source the ROS2 workspace and Python Virtual Environment
source ~/ros2_ws/install/local_setup.bash
source ~/ros2_ws/.venv/bin/activate

# Run your target launch file
ros2 launch oarbot_launch nuc.launch.py