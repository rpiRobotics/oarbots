# Installing 3rd Party Drivers

## Kinova Jaco2 6-DoF Robotic Arm

The Jaco2 drivers come pre-packaged in the [Kinova-Ros2](https://github.com/RRL-ALeRT/kinova-ros2) package, so no further installation is necessary. However, permissions must be properly set up to allow access to the arm via USB. To update USB permission rules, run

```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="22cd", ATTRS{idProduct}=="0000", MODE="0666"' | sudo tee /etc/udev/rules.d/99-kinova-usb.rules
```

followed by 

```bash
sudo udevadm control --reload` and then `sudo udevadm trigger
```

to reload the udev rules. Then, disconnect and reconnect the Jaco2 arm. To verify everything is working, build the package, source it in the terminal, and run 

```bash
ros2 launch kinova_bringup kinova_robot_launch.py
```

No errors should be reported.

## Azure Kinect

Drivers do not come included in the [Azure_Kinect_ROS_Driver](https://github.com/rpiRobotics/Azure_Kinect_ROS_Driver) package. Instead, they need to be downloaded and installed manually. 

To download the Azure Kinect SDK, run

```bash
wget https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4a1.4-dev/libk4a1.4-dev_1.4.1_amd64.deb
```

and

```bash
wget https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4a1.4/libk4a1.4_1.4.1_amd64.deb
```

into a directory (I used ~/Downloads). Ensure these two `.deb` files are the only `.deb` files in the directory. In this same directory, install the SDK by running

```bash
sudo apt install ./*.deb
```

Next, download the body tracking SDK by running

```bash
wget https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4abt1.1-dev/libk4abt1.1-dev_1.1.2_amd64.deb
```

and

```bash
wget https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4abt1.1/libk4abt1.1_1.1.2_amd64.deb
```

Before installing, ensure only the body tracking `.deb` files are the only `.deb` files present in the directory (this is likely *not* the case as you just downloaded two other `.deb` files before these). In this same directory, install the body tracking SDK by running

```bash
sudo apt install ./*.deb
```

To allow Azure Kinect access without root permissions, copy the following file into the directory `/etc/udev/rules.d/`: [https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/scripts/99-k4a.rules](https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/scripts/99-k4a.rules).

To verify the driver is properly installed and to learn more, follow the [usage guide](https://github.com/rpiRobotics/Azure_Kinect_ROS_Driver/blob/jazzy/docs/usage.md).