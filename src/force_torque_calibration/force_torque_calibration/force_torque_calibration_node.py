import rclpy
from rclpy.node import Node

class ForceTorqueCalibration(Node):
    def __init__(self) -> None:
        super().__init__("force_torque_calibration")


        