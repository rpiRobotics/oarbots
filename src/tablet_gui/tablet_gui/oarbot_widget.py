from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QLabel
)
from PySide6.QtGui import QFont
from PySide6.QtCore import Qt, QTimer

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.status_indicators import StatusIndicators
from tablet_gui.push_button import PushButton
from tablet_gui.toggle_button import ToggleButton
from tablet_gui.horizontal_slider import HorizontalSlider

class OarbotWidget(QWidget):
    def __init__(self, oarbot_namespace: str, display_name: str, ros_gui_node: RosGuiNode) -> None:
        super().__init__()

        self.oarbot_namespace = oarbot_namespace
        self.display_name = display_name
        self.ros_gui_node = ros_gui_node

        # Set up the ROS2 node to know about this OARBot
        ros_gui_node.add_oarbot(oarbot_namespace)

        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop)
        layout.setContentsMargins(0, 16, 0, 16)

        self.oarbot_main_text = QLabel(self.display_name)
        self.oarbot_main_text.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.oarbot_main_text.setFont(QFont("Roboto", 24))
        layout.addWidget(self.oarbot_main_text)

        self.oarbot_status = StatusIndicators(
            display_name_to_topic_node_name={
                "Dingo Base": oarbot_namespace + "/dingo/sensors/imu_0/data",
                "Kinova Arm": oarbot_namespace + "/kinova/kinova_arm",
                "Azure Kinect": oarbot_namespace + "/azure_kinect/k4a_bridge",
                "Force Torque": oarbot_namespace + "/rokubi_force_torque_publisher",
                "MoveIt": oarbot_namespace + "/move_group"
            },
            ros_gui_node=self.ros_gui_node
        )
        layout.addWidget(self.oarbot_status)

        translation_rotation_toggle_layout = QHBoxLayout()
        self.translation_toggle_button = ToggleButton("Translation\nEnabled", self.handle_translation_toggle_button, font_size=14)
        self.translation_toggle_button.click()
        translation_rotation_toggle_layout.addWidget(self.translation_toggle_button)
        self.rotation_toggle_button = ToggleButton("Rotation\nEnabled", self.handle_rotation_toggle_button, font_size=14)
        self.rotation_toggle_button.click()
        translation_rotation_toggle_layout.addWidget(self.rotation_toggle_button)
        layout.addLayout(translation_rotation_toggle_layout)

        self.arm_home_button = PushButton("Arm Home", self.handle_arm_home)
        layout.addWidget(self.arm_home_button)

        self.base_home_button = PushButton("Base Home", self.handle_base_home)
        layout.addWidget(self.base_home_button)

        self.arm_enable_button = ToggleButton("Arm Disabled", self.handle_arm_toggle_button)
        layout.addWidget(self.arm_enable_button)

        self.base_enable_button = ToggleButton("Base Disabled", self.handle_base_toggle_button)
        layout.addWidget(self.base_enable_button)

        self.finger_position_slider = HorizontalSlider(0, 100, self.ros_gui_node.get_cur_finger_percent(self.oarbot_namespace), self.handle_finger_slider)
        layout.addWidget(self.finger_position_slider)
        finger_position_slider_text = QLabel("Arm Finger Positions")
        finger_position_slider_text.setAlignment(Qt.AlignmentFlag.AlignCenter)
        finger_position_slider_text.setFont(QFont("Roboto", 12))
        layout.addWidget(finger_position_slider_text)

        # Set up a slider to update the finger position slider
        self.finger_position_timer = QTimer(self)
        self.finger_position_timer.setInterval(100)
        self.finger_position_timer.timeout.connect(self.update_finger_position_slider)
        self.finger_position_timer.start()
        self.do_finger_position_slider_update = True # If the user is clicking on the slider, this should be set to false
        self.finger_position_slider.sliderPressed.connect(lambda : setattr(self, "do_finger_position_slider_update", False))

        self.body_joint_following_enable_button = ToggleButton("Body Joint Following Disabled", self.handle_body_joint_following_toggle_button, 14)
        layout.addWidget(self.body_joint_following_enable_button)

        self.admittance_control_enable_button = ToggleButton("Admittance Control Disabled", self.handle_admittance_control_toggle_button, 14)
        layout.addWidget(self.admittance_control_enable_button)

    def resizeEvent(self, event):
        # Capture this event to set all button widths to be 90% of the available width
        # Each button is set to 90% width then moved over 5% of the available width to center it
        self.arm_home_button.setFixedWidth(int(self.width() * 0.9))
        self.arm_home_button.move(
            int(self.width() * 0.05),
            self.arm_home_button.y()
        )
        self.base_home_button.setFixedWidth(int(self.width() * 0.9))
        self.base_home_button.move(
            int(self.width() * 0.05),
            self.base_home_button.y()
        )
        self.arm_enable_button.setFixedWidth(int(self.width() * 0.9))
        self.arm_enable_button.move(
            int(self.width() * 0.05),
            self.arm_enable_button.y()
        )
        self.base_enable_button.setFixedWidth(int(self.width() * 0.9))
        self.base_enable_button.move(
            int(self.width() * 0.05),
            self.base_enable_button.y()
        )
        self.finger_position_slider.setFixedWidth(int(self.width() * 0.9))
        self.finger_position_slider.move(
            int(self.width() * 0.05),
            self.finger_position_slider.y()
        )
        self.body_joint_following_enable_button.setFixedWidth(int(self.width() * 0.9))
        self.body_joint_following_enable_button.move(
            int(self.width() * 0.05),
            self.body_joint_following_enable_button.y()
        )
        self.admittance_control_enable_button.setFixedWidth(int(self.width() * 0.9))
        self.admittance_control_enable_button.move(
            int(self.width() * 0.05),
            self.admittance_control_enable_button.y()
        )

        super().resizeEvent(event)

    def handle_translation_toggle_button(self, toggled: bool) -> None:
        self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].translation_enabled = toggled
        if self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].admittance_control_enabled:
            self.ros_gui_node.disable_admittance_control(self.oarbot_namespace)
            self.ros_gui_node.enable_admittance_control(self.oarbot_namespace)
        self.translation_toggle_button.setText("Translation\nEnabled" if toggled else "Translation\nDisabled")

    def handle_rotation_toggle_button(self, toggled: bool) -> None:
        self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].rotation_enabled = toggled
        if self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].admittance_control_enabled:
            self.ros_gui_node.disable_admittance_control(self.oarbot_namespace)
            self.ros_gui_node.enable_admittance_control(self.oarbot_namespace)
        self.rotation_toggle_button.setText("Rotation\nEnabled" if toggled else "Rotation\nDisabled")

    def handle_arm_home(self) -> None:
        pass

    def handle_base_home(self) -> None:
        pass

    def handle_arm_toggle_button(self, toggled: bool) -> None:
        self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].arm_enabled = toggled
        self.arm_enable_button.setText("Arm Enabled" if toggled else "Arm Disabled")

    def handle_base_toggle_button(self, toggled: bool) -> None:
        self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].base_enabled = toggled
        self.base_enable_button.setText("Base Enabled" if toggled else "Base Disabled")

    def handle_finger_slider(self) -> None:
        value = self.finger_position_slider.value()
        self.ros_gui_node.set_finger_position(self.oarbot_namespace, value)
        self.do_finger_position_slider_update = True

    def update_finger_position_slider(self) -> None:
        if self.do_finger_position_slider_update:
            percent_open = self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].finger_position_percent
            self.finger_position_slider.setValue(percent_open)

    def handle_body_joint_following_toggle_button(self, toggled: bool) -> None:
        self.ros_gui_node.oarbot_settings_dict[self.oarbot_namespace].body_joint_following_enabled = toggled
        self.body_joint_following_enable_button.setText("Body Joint Following Enabled" if toggled else "Body Joint Following Disabled")

    def handle_admittance_control_toggle_button(self, toggled: bool) -> None:
        if toggled:
            self.ros_gui_node.enable_admittance_control(self.oarbot_namespace)
        else:
            self.ros_gui_node.disable_admittance_control(self.oarbot_namespace)
        self.admittance_control_enable_button.setText("Admittance Control Enabled" if toggled else "Admittance Control Disabled")