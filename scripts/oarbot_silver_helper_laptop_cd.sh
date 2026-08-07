#!/bin/bash

source /opt/ros/jazzy/setup.bash

# Update 3'rd party / externally managed packages
cd /home/oarbot_silver/ros2_ws/src
vcs import < ../packages.repos .
vcs pull .
cd ..

echo "Updated 3rd party dependencies"

# Update main oarbot repo
git pull

echo "Updated main repository"

# Pull down services and scripts
cp scripts/oarbot_silver_helper_laptop_launch.sh /usr/local/bin/oarbot_silver_helper_laptop_launch.sh
chmod +x /usr/local/bin/oarbot_silver_helper_laptop_launch.sh
cp config/oarbot_silver_helper_laptop_launch.service /etc/systemd/system/oarbot_silver_helper_laptop_launch.service

cp config/xorg-headless.conf /etc/X11/xorg-headless.conf
cp config/xorg-headless.service /etc/systemd/system/xorg-headless.service

echo "Updated services"


# Update ros-managed dependencies
rosdep update
rosdep install --from-paths src --ignore-src -y

echo "Updated dependencies"

# Build everything
colcon build

echo "Built the ROS2 packages"

# Update this service and script
cp scripts/oarbot_silver_helper_laptop_cd.sh /usr/local/bin/oarbot_silver_helper_laptop_cd.sh
chmod +x /usr/local/bin/oarbot_silver_helper_laptop_cd.sh
cp config/oarbot_silver_helper_laptop_cd.service /etc/systemd/system/oarbot_silver_helper_laptop_cd.service

echo "Updated CD service"

# Reload all services
systemctl daemon-reload

echo "Reloaded systemctl daemon"
echo "Done with CD. Exiting..."