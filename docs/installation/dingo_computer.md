# Clearpath Dingo Onboard Computer Installation Instructions

## Installing/Upgrading Dingo Robot to ROS2

Follow the instructions on [https://docs.clearpathrobotics.com/docs/ros/installation/robot/](https://docs.clearpathrobotics.com/docs/ros/installation/robot/). However, the following extra notes should help:

- When installing from the ISO, the computer needs to be connected to the internet via Ethernet from the ETH1 port. When looking towards the two Ethernet ports on the computer (looking at the port you plug into, and not the back of the port), this is the one on the right
- Settings from the install script should be the default/recommended ones. For the serial number, this is the last 4 digits from the writing on the top. For example, for Dingo base 2, this is: 0051
- Firmware upgrade page will return to the "select a device" page once done; this is normal. Just exit out and continue with the instructions
- "Step 4: Package Install" seems to be just a repeat of what you've already done, so no need to do it again
- To test the robot, try the tutorial [Driving a Robot](https://docs.clearpathrobotics.com/docs/ros/tutorials/driving). Make sure to use the namespaced version with the correct namespace (something like `do100_0051`) *and* make sure to turn on the motors. This is the button on the front panel with an M and a circle around it with lines going out horizontally

## Downloading Code

Run 

```bash
mkdir ~/ros2_ws && cd ~/ros2_ws
```

This creates the directory `ros2_ws` in the `/home/robot` directory.

Run

```bash
git clone https://github.com/rpiRobotics/oarbots .
```

This copies the repository contents into `~/ros2_ws`. To verify, run `ls` and you should see folders and files present in the directory, including (but not limited to) `src/`, `.gitignore`, `LICENSE.md`, and `README.md`. If you only see a folder named `oarbots/`, you did not include the trailing period ("`.`") at the end of the `git clone` command. Run `rm -rf oarbots` and run the command again, making sure to copy it fully.

## Installing Dependencies

To instal 3'rd party dependencies *not* available through package managers, run

```bash
cd src && vcs import < ../packages.repos .
```

After, run

```bash
cd ..
```

to get back to the `ros2_ws` directory.

To install dependencies that *are* available through package managers, run

```bash
rosdep install --from-paths src --ignore-src -y
```

If not already, you may need to run

```bash
sudo rosdep init
```

and


```bash
rosdep update
```

to setup and update rosdep's dependency list.

## Building the packages

At this point, you should be able to run

```bash
colcon build --packages-select dingo_tf_relay
```

in the `~/ros2_ws` directory. No errors should occur.

## Configuring the Dingo Robot

Replace the contents of `/etc/clearpath/robot.yaml` with the contents of [robot.yaml](robot.yaml), making sure to replace any filler content (i.e. replace `<LAST_FOUR_OF_SERIAL_HERE>`, `<NAMESPACE_HERE>`, etc.).