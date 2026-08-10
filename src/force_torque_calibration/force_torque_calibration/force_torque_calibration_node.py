import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from rcl_interfaces.msg import ParameterDescriptor
import csv
from kinova_msgs.action import ArmJointAngles
from kinova_msgs.msg import JointAngles
from math import pi

class ForceTorqueCalibration(Node):
    def __init__(self) -> None:
        super().__init__("force_torque_calibration")

        joint_angles_csv_description = ParameterDescriptor(description="Path to the csv file containing rows of 6 numbers, describing the joint angles of the Kinova arm")
        self.declare_parameter("joint_angles_csv", value="", descriptor=joint_angles_csv_description)
        joint_angles_csv_path = self.get_parameter("joint_angles_csv").get_parameter_value().string_value
        if joint_angles_csv_path == "":
            raise ValueError("joint_angles_csv is empty")

        # Get the data from the CSV into a list
        self.joint_angles_list: list[JointAngles] = []
        with open(joint_angles_csv_path, newline="") as file:
            data = csv.reader(file)
            for row in data:
                # Make sure to convert from radians to degrees
                joint_angles_list = [float(angle) * 180 / pi for angle in row]
                cur_joint_angles = JointAngles()
                cur_joint_angles.joint1 = joint_angles_list[0]
                cur_joint_angles.joint2 = joint_angles_list[1]
                cur_joint_angles.joint3 = joint_angles_list[2]
                cur_joint_angles.joint4 = joint_angles_list[3]
                cur_joint_angles.joint5 = joint_angles_list[4]
                cur_joint_angles.joint6 = joint_angles_list[5]
                self.joint_angles_list.append(cur_joint_angles)

        # Start the action client
        self.joint_angles_action = ActionClient(self, ArmJointAngles, "kinova/j2n6s300_driver/joint_angles")
        if not self.joint_angles_action.wait_for_server(timeout_sec=5.0):
            self.get_logger().error("Kinova joint angle action server is not available")
            raise RuntimeError("Kinova joint angle action server is not available")

    def move_arm_to_state(self, joint_angles: JointAngles) -> None:
        self.get_logger().info(f"Starting to move to {joint_angles}")
        goal_msg = ArmJointAngles.Goal()
        goal_msg.angles = joint_angles

        future = self.joint_angles_action.send_goal_async(goal_msg)
        self.get_logger().info("Sending action goal")
        rclpy.spin_until_future_complete(self, future)

        goal_handle = future.result()
        if not goal_handle:
            self.get_logger().error("Action goal was rejected by the server")
            return

        result_future = goal_handle.get_result_async()
        rclpy.spin_until_future_complete(self, result_future)
        self.get_logger().info("Action completed")
