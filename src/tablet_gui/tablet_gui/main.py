import sys
import threading
import rclpy
from PySide6.QtWidgets import QApplication
from gui import AppWindow
from ros_gui import RosGui

def spin_ros(node, shutdown_event):
    try:
        # We run spin_once() instead of spin() to periodically check for a shutdown event
        while not shutdown_event.is_set():
            rclpy.spin_once(node, timeout_sec=0.1)  
    finally:
        node.destroy_node()
        rclpy.shutdown()


def main(args=None):
    rclpy.init(args=args)
    node = RosGui()

    shutdown_event = threading.Event()

    # Start the ROS2 node in a separate thread
    ros_thread = threading.Thread(target=spin_ros, args=(node, shutdown_event))
    ros_thread.start()

    # QT will be on the main thread
    app = QApplication(sys.argv)
    window = AppWindow(node, shutdown_event)
    window.show()

    sys.exit(app.exec())

    ros_thread.join()

    print("ROS Node and GUI have been shut down.")

if __name__ == '__main__':
    main()