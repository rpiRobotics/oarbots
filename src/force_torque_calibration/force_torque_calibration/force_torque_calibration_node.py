import rclpy
from rclpy.node import Node
from rcl_interfaces.msg import ParameterDescriptor
import csv

class ForceTorqueCalibration(Node):
    def __init__(self) -> None:
        super().__init__("force_torque_calibration")

        joint_states_csv_description = ParameterDescriptor(description="Path to the csv file containing rows of 6 numbers, describing the joint states of the Kinova arm")
        self.declare_parameter("joint_states_csv", value="", descriptor=joint_states_csv_description)
        joint_states_csv_path = self.get_parameter("joint_states_csv").get_parameter_value().string_value
        if joint_states_csv_path == "":
            raise ValueError("joint_states_csv is empty")

        self.joint_states_list: list[list[float]] = []
        with open(joint_states_csv_path, newline="") as file:
            data = csv.reader(file)
            for row in data:
                self.joint_states_list.append([float(num) for num in row])

        