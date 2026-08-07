# Set up the display to use with Azure Kinect

Edit `/etc/gdm3/custom.conf` and ensure the following lines are uncommented:

```conf
[daemon]
AutomaticLoginEnable=true
AutomaticLogin=nuc
WaylandEnable=false
```

To enable the changes, SSH into the NUC and run

```bash
sudo systemctl restart gdm3
```

Copy the service and shell scripts into their directories