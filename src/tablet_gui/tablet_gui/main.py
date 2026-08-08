import sys
import threading
from threading import Event
import rclpy
from PySide6.QtWidgets import QApplication
from tablet_gui.gui import AppWindow
from tablet_gui.ros_gui_node import RosGuiNode

def spin_ros(node: RosGuiNode, shutdown_event: Event):
    try:
        # We run spin_once() instead of spin() to periodically check for a shutdown event
        while not shutdown_event.is_set():
            rclpy.spin_once(node, timeout_sec=0.1)  
    finally:
        node.destroy_node()
        rclpy.shutdown()


def main(args=None):
    rclpy.init(args=args)
    ros_node = RosGuiNode()

    shutdown_event = threading.Event()

    # Start the ROS2 node in a separate thread
    ros_thread = threading.Thread(target=spin_ros, args=(ros_node, shutdown_event))
    ros_thread.start()

    # QT will be on the main thread
    app = QApplication(sys.argv)
    window = AppWindow(ros_node, shutdown_event)
    
    window.showMaximized()
    app.exec()

    ros_thread.join()

    print("ROS Node and GUI have been shut down.")

if __name__ == '__main__':
    main()