#!/bin/bash
set -e

# In order for the Azure Kinect node to launch, a display needs to be connected. This can be done by SSH'ing
# with -X, but we will use a virtual display backed by the NVIDIA GPU
export DISPLAY=:2

# Force GLX/EGL to use the NVIDIA vendor library rather than Mesa/llvmpipe.
# This matters on laptops with hybrid Intel+NVIDIA graphics, where glvnd
# could otherwise pick the wrong vendor.
export __GLX_VENDOR_LIBRARY_NAME=nvidia
export __NV_PRIME_RENDER_OFFLOAD=1
export __VK_LAYER_NV_optimus=NVIDIA_only

# Source the global ROS2 installation
source /opt/ros/jazzy/setup.bash

# Use the correct DDS provider
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp 

# Source your specific workspace setup file
source ~/ros2_ws/install/local_setup.bash

# Run your target launch file
ros2 launch oarbot_launch oarbot_silver_helper_laptop.launch.py