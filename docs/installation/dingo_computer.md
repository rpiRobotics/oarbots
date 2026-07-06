# Clearpath Dingo Onboard Computer Installation Instructions

## Installing/Upgrading Dingo Robot to ROS2

<!-- TODO -->

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

## Configuring the Dingo Robot

