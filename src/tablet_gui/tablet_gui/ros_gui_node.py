from rclpy.node import Node

class RosGuiNode(Node):
    def __init__(self) -> None:
        super().__init__("ros_gui_node")
        # TODO: This node should have an add_listener(oarbot_namespace) function that will add listeners to all the topics it needs so it doesn't have to wait on receiving data

        self.get_logger().info("Started ROS2 Node")

    def is_node_active(self, node_name: str) -> bool:
        return node_name in self.get_fully_qualified_node_names()

    def get_cur_finger_percent(self, oarbot_namespace: str) -> int:
        return 50