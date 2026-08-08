from rclpy.node import Node

class RosGuiNode(Node):
    def __init__(self) -> None:
        super().__init__("ros_gui_node")

        self.get_logger().info("Started ROS2 Node")