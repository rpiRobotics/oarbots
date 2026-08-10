import rclpy
import time
from geometry_msgs.msg import Vector3Stamped

from force_torque_calibration.force_torque_calibration_node import ForceTorqueCalibration

def main(args=None) -> None:
    rclpy.init(args=args)

    print("-----------WARNING-----------")
    print("Unplug the Azure Kinect camera (this requires a restart of the node after running this calibration) and remove the both the Azure Kinect and force torque cables from the Zip-Tie")
    input("Press Enter to continue...")

    node = ForceTorqueCalibration()

    for i, joint_angles in enumerate(node.joint_angles_list):
        node.get_logger().info(f"Moving arm to position #{i}")
        node.move_arm_to_state(joint_angles)

        node.get_logger().info("Waiting for arm to finish shaking")
        time.sleep(5)

        node.get_logger().info("Collecting force torque data")
        node.collect_force_torque_data()

        node.get_logger().info("Getting gravity in force torque frame")
        gravity_vector = Vector3Stamped()
        for i in range(10):
            try:
                gravity_vector = node.get_gravity_vector()
            except LookupError as ex:
                node.get_logger().info("Trying to get transform again")
                time.sleep(0.5)
                continue

            break

        if gravity_vector == Vector3Stamped():
            raise LookupError("Failed to get transform 10 times; exiting")

        node.get_logger().info("")