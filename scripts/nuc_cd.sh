#!/bin/bash

source /opt/ros/jazzy/setup.bash

# Update 3'rd party / externally managed packages
cd /home/nuc/ros2_ws/src
vcs import < ../packages.repos .
vcs pull .
cd ..

echo "Updated 3rd party dependencies"

# Update main oarbot repo
git pull

echo "Updated main repository"

# Pull down services and scripts
cp scripts/nuc_launch.sh /usr/local/bin/nuc_launch.sh
chmod +x /usr/local/bin/nuc_launch.sh
cp config/nuc_launch.service /etc/systemd/system/nuc_launch.service

echo "Updated services"

# Reload all services
systemctl daemon-reload

echo "Reloaded systemctl daemon"

# Update ros-managed dependencies
rosdep update
rosdep install --from-paths src --ignore-src -y

echo "Updated dependencies"

# Build everything
colcon build

echo "Built the ROS2 packages"

echo "Done with CD. Exiting..."