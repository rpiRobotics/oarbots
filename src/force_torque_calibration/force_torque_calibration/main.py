import rclpy

from force_torque_calibration.force_torque_calibration_node import ForceTorqueCalibration

def main(args=None) -> None:
    rclpy.init(args=args)

    force_torque_calibration = ForceTorqueCalibration()

    for joint_angles in force_torque_calibration.joint_angles_list:
        force_torque_calibration.move_arm_to_state(joint_angles)