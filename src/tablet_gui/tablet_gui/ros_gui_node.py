from rclpy.subscription import Subscription
from rclpy.publisher import Publisher
from rclpy.node import Node
from kinova_msgs.msg import FingerPosition, PoseVelocityWithFingers, PoseVelocity
from geometry_msgs.msg import Twist, TwistStamped
from math import pi

class RosGuiNode(Node):
    def __init__(self) -> None:
        super().__init__("ros_gui_node")
        # TODO: This node should have an add_listener(oarbot_namespace) function that will add listeners to all the topics it needs so it doesn't have to wait on receiving data

        self.get_logger().info("Started ROS2 Node")
        self.oarbot_settings_dict: dict[str, OarbotSettings] = dict()
        self.finger_position_subscribers: dict[str, Subscription] = dict()
        self.finger_position_publishers: dict[str, Publisher] = dict()
        self.arm_velocity_publishers: dict[str, Publisher] = dict()
        self.base_velocity_publishers: dict[str, Publisher] = dict()

        self.spacenav_subscriber = self.create_subscription(
            msg_type=Twist,
            topic="spacenav/twist",
            callback=self.spacenav_callback,
            qos_profile=5
        )


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

    def add_oarbot(self, oarbot_name: str) -> None:
        self.oarbot_settings_dict[oarbot_name] = OarbotSettings()

        self.finger_position_subscribers[oarbot_name] = self.create_subscription(
            msg_type=FingerPosition,
            topic=oarbot_name + "/kinova/j2n6s300_driver/out/finger_position",
            callback=lambda msg : self.finger_position_callback(oarbot_name, msg),
            qos_profile=3
        )
        self.finger_position_publishers[oarbot_name] = self.create_publisher(
            msg_type=PoseVelocityWithFingers,
            topic=oarbot_name + "/kinova/j2n6s300_driver/in/cartesian_velocity_with_fingers",
            qos_profile=1
        )
        self.arm_velocity_publishers[oarbot_name] = self.create_publisher(
            msg_type=PoseVelocity,
            topic=oarbot_name + "/kinova/j2n6s300_driver/in/cartesian_velocity",
            qos_profile=1
        )
        self.base_velocity_publishers[oarbot_name] = self.create_publisher(
            msg_type=TwistStamped,
            topic=oarbot_name + "/dingo/cmd_vel",
            qos_profile=10
        )

    def finger_position_callback(self, oarbot_name: str, msg) -> None:
        avg_finger_position = (msg.finger1 + msg.finger2 + msg.finger3) / 3

        # This formula comes from the kinova library; to learn more, search for the variable finger_conv_ratio_ in kinova_arm.cpp
        finger_position_angle_rad = avg_finger_position * (80.0 / 6800.0) * pi  / 180.0
        # These come from kinova_common.xacro
        min_angle_rad = 0.0
        max_angle_rad = 1.51

        # Sometimes the angle given is beyond the range; clamp it
        clamped_finger_positon_angle_rad = max(min_angle_rad, min(finger_position_angle_rad, max_angle_rad))

        finger_position_open_percent = int(100 - 100 * (clamped_finger_positon_angle_rad - min_angle_rad) / (max_angle_rad - min_angle_rad))

        self.oarbot_settings_dict[oarbot_name].finger_position_percent = finger_position_open_percent

    def set_finger_position(self, oarbot_name: str, finger_position_percent: int) -> None:
        self.get_logger().info(f"Setting finger position to {finger_position_percent}")
        # Ensure the value is clamped between 100 and 0
        clamped_percent = max(0, min(100, finger_position_percent))

        msg = PoseVelocityWithFingers()
        msg.fingers_closure_percentage = float(clamped_percent)

        self.finger_position_publishers[oarbot_name].publish(msg)
        self.get_logger().info(f"Published {msg} to {oarbot_name}")

    def spacenav_callback(self, msg: Twist) -> None:
        for oarbot_namespace, oarbot in self.oarbot_settings_dict.items():
            if oarbot.arm_enabled:
                arm_msg = PoseVelocity()

                if oarbot.translation_enabled:
                    arm_msg.twist_linear_x = msg.linear.x
                    arm_msg.twist_linear_y = msg.linear.y
                    arm_msg.twist_linear_z = msg.linear.z

                    # For some reason, oarbot_silver has x-y values flipped
                    if oarbot_namespace == "/oarbot_silver":
                        arm_msg.twist_linear_x *= -1
                        arm_msg.twist_linear_y *= -1
                if oarbot.rotation_enabled:
                    # Here, we need to swap the x and y angular values; just how the kinova is set up, I guess...
                    arm_msg.twist_angular_x = msg.angular.y
                    arm_msg.twist_angular_y = msg.angular.x
                    arm_msg.twist_angular_z = msg.angular.z

                    # For some reason, oarbot_blue has x-y values flipped
                    if oarbot_namespace == "/oarbot_blue":
                        arm_msg.twist_angular_x *= -1
                        arm_msg.twist_angular_y *= -1
    
                self.arm_velocity_publishers[oarbot_namespace].publish(arm_msg)

            if oarbot.base_enabled:
                base_msg = TwistStamped()
                base_msg.header.stamp = self.get_clock().now().to_msg()

                if oarbot.translation_enabled:
                    base_msg.twist.linear = msg.linear
                if oarbot.rotation_enabled:
                    base_msg.twist.angular = msg.angular

                self.base_velocity_publishers[oarbot_namespace].publish(base_msg)


class OarbotSettings():
    def __init__(self) -> None:
        self.translation_enabled = True
        self.rotation_enabled = True
        self.arm_enabled = False
        self.base_enabled = False
        self.body_joint_following_enabled = False
        self.admittance_control_enabled = False
        self.finger_position_percent = 0