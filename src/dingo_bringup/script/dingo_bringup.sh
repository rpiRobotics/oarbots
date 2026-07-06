#!/bin/bash

# Stop execution if any setup command fails
set -e

# Source the global ROS 2 installation
source /opt/ros/jazzy/setup.bash

# Source your local workspace using absolute paths
source /home/robot/ros2_ws/install/local_setup.bash

# Execute the launch file
exec /opt/ros/jazzy/bin/ros2 launch dingo_bringup dingo_launch.py dingo_name:='<DINGO_NAME_HERE>'