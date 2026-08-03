## Setup a Virtual Environment for Python

The package ros2-aruco-pose-estimation requires a newer OpenCV version than is provided by apt for system-wide usage. Therefore, in order to run this node, a python virtual environment that contains the newer OpenCV version must be created and sourced. 

Allow the creation of virtual environments by running

```bash
sudo apt install python3.12-venv
```

Create the virtual environment

```bash
python3 -m venv ~/ros2_ws/.venv --system-site-packages
```

Where `--system-site-packages` gives the virtual environment access to all system-wide packages. To use the virtual environment, run

```bash
source ~/ros2_ws/.venv/bin/activate
```

To install the needed OpenCV version, run

```bash
pip install opencv-python==4.7.0.72
```

You can now run the node as normal. Whenever the node is run, the virtual environment must be sourced.