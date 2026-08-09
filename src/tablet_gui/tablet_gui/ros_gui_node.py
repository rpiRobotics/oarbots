from rclpy.node import Node

class RosGuiNode(Node):
    def __init__(self) -> None:
        super().__init__("ros_gui_node")
        # TODO: This node should have an add_listener(oarbot_namespace) function that will add listeners to all the topics it needs so it doesn't have to wait on receiving data

        self.get_logger().info("Started ROS2 Node")

    def active_node_or_topic(self, topic_node_name: str) -> bool:
        """
        Returns if the given node or topic name is active. This is used in status_indicator.py to allow the user to pass in either a node or topic name
        
        Specifically, the dingo status indicator needs to be a topic name because of the ros domain bridge acting on its nodes
        """
        return self.is_node_active(topic_node_name) or self.topic_has_publisher(topic_node_name)

    def is_node_active(self, node_name: str) -> bool:
        return node_name in self.get_fully_qualified_node_names()

    def topic_has_publisher(self, topic_name: str) -> bool:
        return len(self.get_publishers_info_by_topic(topic_name=topic_name)) > 0

    def get_cur_finger_percent(self, oarbot_namespace: str) -> int:
        return 50