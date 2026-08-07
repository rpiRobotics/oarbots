#!/bin/bash

source /opt/ros/jazzy/setup.bash

# Update 3'rd party / externally managed packages
cd /home/tablet/ros2_ws/src
vcs import < ../packages.repos .
vcs pull .
cd ..

echo "Updated 3rd party dependencies"

# Update main oarbot repo
git pull

echo "Updated main repository"

# Update ros-managed dependencies
rosdep update
rosdep install --from-paths src --ignore-src -y

echo "Updated dependencies"

# Build everything
colcon build

echo "Built the ROS2 packages"

echo "Done with CD. Exiting..."