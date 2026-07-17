## Installing the Startup Service

Copy `oarbot_silver_helper_laptop_launch.service` into `/etc/systemd/system/`, and copy `oarbot_silver_helper_laptop_launch.sh` into `/usr/local/bin/`. To make the script executable, run

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