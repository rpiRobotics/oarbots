import rclpy

from force_torque_calibration.force_torque_calibration_node import ForceTorqueCalibration

def main(args=None) -> None:
    rclpy.init(args=args)

    force_torque_calibration = ForceTorqueCalibration()

    rclpy.spin(force_torque_calibration)
    rclpy.shutdown()