## Installing a Virtual Display Startup Service

In order for the Azure Kinect node to run, it needs access to a display. This can be fulfilled in several ways:

1. Launching the ROS2 node from a desktop environment
2. SSH'ing into the machine with the `-X` flag
3. Using a virtual display

As we want the service to launch on startup, we will add a virtual display service that starts alongside the startup service below.

To add dummy video drivers, which are needed, run

```bash
sudo apt install xserver-xorg-video-dummy
```

Copy [xorg-headless.conf](xorg-headless.conf) into `/etc/X11/`. This serves as the dummy display configuration file. To launch a display with this configuration, we create a systemd service. Copy [xorg-headless.service](xorg-headless.service) into `/etc/systemd/system/`. Run

```bash
sudo systemctl daemon-reload
```

followed by

```bash
sudo systemctl enable xorg-headless.service
```

To start the service, run


```bash
sudo systemctl start xorg-headless.service
```

You should now be able to run

```bash
sudo systemctl status xorg-headless.service
```

to view the status of the service (some warnings are okay, but the service should be up). To ensure the display is up, run

```bash
DISPLAY=:2 glxinfo | grep -E "OpenGL|renderer"
```

If you run into an error saying something like the screen is already running, it is likely there is already a display connected to :2, which is what this one is connecting to. To resolve this, modify the `.conf`, `.service`, and (for the next step) the oarbot `.sh` file to use a different display number.

## Installing the Startup Service

Copy [oarbot_silver_helper_laptop_launch.service](oarbot_silver_helper_laptop_launch.service) into `/etc/systemd/system/`, and copy [oarbot_silver_helper_laptop_launch.sh](oarbot_silver_helper_laptop_launch.sh) into `/usr/local/bin/`. To make the script executable, run

```bash
sudo chmod +x /usr/local/bin/oarbot_silver_helper_laptop_launch.sh
```

To allow the service to automatically start on boot, run

```bash
sudo systemctl daemon-reload
```

followed by

```bash
sudo systemctl enable oarbot_silver_helper_laptop_launch.service
```

As the service does not start automatically without a restart, run

```bash
sudo systemctl start oarbot_silver_helper_laptop_launch.service
```

to start the service now. You can also run

```bash
sudo systemctl status oarbot_silver_helper_laptop_launch.service
```

to view the service status.