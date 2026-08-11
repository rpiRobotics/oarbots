import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from rclpy.qos import qos_profile_sensor_data
from rclpy.time import Time
from rcl_interfaces.msg import ParameterDescriptor
import csv
from kinova_msgs.action import ArmJointAngles
from kinova_msgs.msg import JointAngles
from geometry_msgs.msg import WrenchStamped, Vector3Stamped
from math import pi
from tf2_ros.buffer import Buffer
from tf2_ros.transform_listener import TransformListener
import tf2_geometry_msgs

class ForceTorqueCalibration(Node):
    def __init__(self) -> None:
        super().__init__("force_torque_calibration")

        joint_angles_csv_description = ParameterDescriptor(description="Path to the csv file containing rows of 6 numbers, describing the joint angles of the Kinova arm")
        self.declare_parameter("joint_angles_csv", value="", descriptor=joint_angles_csv_description)
        joint_angles_csv_path = self.get_parameter("joint_angles_csv").get_parameter_value().string_value
        if joint_angles_csv_path == "":
            raise ValueError("joint_angles_csv is empty")

        tf_prefix_description = ParameterDescriptor(description="Prefix for tf frames; should include a trailing slash")
        self.declare_parameter("tf_prefix", value="/", descriptor=tf_prefix_description)
        self.tf_prefix = self.get_parameter("tf_prefix").get_parameter_value().string_value
        if self.tf_prefix == "":
            raise ValueError("If you want no tf prefix, leave the paramter blank or set it to '/'")
        if self.tf_prefix == "/":
            self.get_logger().warn("tf prefix is '/'; continuing...")

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


        # Initialize the tf listener
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

    def move_arm_to_state(self, joint_angles: JointAngles) -> None:
        goal_msg = ArmJointAngles.Goal()
        goal_msg.angles = joint_angles

        future = self.joint_angles_action.send_goal_async(goal_msg)
        rclpy.spin_until_future_complete(self, future)

        goal_handle = future.result()
        if not goal_handle:
            self.get_logger().error("Action goal was rejected by the server")
            return

        result_future = goal_handle.get_result_async()
        rclpy.spin_until_future_complete(self, result_future)

    def collect_force_torque_data(self) -> None:
        self.force_torque_data: list[WrenchStamped] = []
        self.force_torque_subscription = self.create_subscription(
            msg_type=WrenchStamped,
            topic="force_torque_raw",
            callback=self.handle_force_torque_data,
            qos_profile=qos_profile_sensor_data
        )

        # Collect data until we have enough (the subscription is stopped in the callback)
        while len(self.force_torque_data) < 100:
            rclpy.spin_once(self)

    def stop_force_torque_subscription(self) -> None:
        if hasattr(self, "force_torque_subscription"):
            self.destroy_subscription(self.force_torque_subscription)

    def handle_force_torque_data(self, msg: WrenchStamped) -> None:
        self.force_torque_data.append(msg)

        if len(self.force_torque_data) == 100:
            self.stop_force_torque_subscription()

    def get_gravity_vector(self) -> Vector3Stamped:
        try:
            transform = self.tf_buffer.lookup_transform(
                source_frame=self.tf_prefix + "base_link",
                target_frame=self.tf_prefix + "j2n6s300_ft_robot_side_connector",
                time=Time()
            )

            gravity_vector_base_link = Vector3Stamped()
            gravity_vector_base_link.header.frame_id = self.tf_prefix + "base_link"
            gravity_vector_base_link.vector.x = 0.0
            gravity_vector_base_link.vector.y = 0.0
            gravity_vector_base_link.vector.z = -9.80665

            gravity_vector_force_torque = tf2_geometry_msgs.do_transform_vector3(gravity_vector_base_link, transform)

            return gravity_vector_force_torque



        except Exception as ex:
            self.get_logger().error(f"Could not get the transformation from {self.tf_prefix + "base_link"} to {self.tf_prefix + "j2n6s300_ft_robot_side_connector"}")
            raise LookupError(f"Could not get the transformation from {self.tf_prefix + "base_link"} to {self.tf_prefix + "j2n6s300_ft_robot_side_connector"}")

    