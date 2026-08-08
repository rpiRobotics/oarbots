from rclpy.node import Node

class RosGuiNode(Node):
    def __init__(self) -> None:
        super().__init__("ros_gui_node")

        self.get_logger().info("Started ROS2 Node")

    def is_node_active(self, node_name: str) -> bool:
        return node_name in self.get_fully_qualified_node_names()