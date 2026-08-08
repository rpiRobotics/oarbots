from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLabel
)
from PySide6.QtGui import QFont
from PySide6.QtCore import Qt

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.oarbot_status import OarbotStatus
from tablet_gui.push_button import PushButton
from tablet_gui.toggle_button import ToggleButton
from tablet_gui.horizontal_slider import HorizontalSlider

class OarbotWidget(QWidget):
    def __init__(self, oarbot_namespace: str, display_name: str, ros_gui_node: RosGuiNode) -> None:
        super().__init__()

        self.oarbot_namespace = oarbot_namespace
        self.display_name = display_name
        self.ros_gui_node = ros_gui_node

        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop)
        layout.setContentsMargins(0, 16, 0, 16)

        self.oarbot_main_text = QLabel(self.display_name)
        self.oarbot_main_text.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.oarbot_main_text.setFont(QFont("Roboto", 24))
        layout.addWidget(self.oarbot_main_text)

        self.oarbot_status = OarbotStatus(
            display_name_to_node_name={
                "Dingo Base": "dingo",
                "Kinova Arm": "kinova",
                "Force Torque": "ft",
                "MoveIt": "moveit"
            },
            ros_gui_node=self.ros_gui_node
        )
        layout.addWidget(self.oarbot_status)

        self.arm_home_button = PushButton("Arm Home", self.handle_arm_home)
        layout.addWidget(self.arm_home_button)

        self.base_home_button = PushButton("Base Home", self.handle_base_home)
        layout.addWidget(self.base_home_button)

        self.arm_enabled = False
        self.arm_enable_button = ToggleButton("Arm Disabled", self.handle_arm_toggle_button)
        layout.addWidget(self.arm_enable_button)

        self.base_enabled = False
        self.base_enable_button = ToggleButton("Base Disabled", self.handle_base_toggle_button)
        layout.addWidget(self.base_enable_button)

        self.finger_position_slider = HorizontalSlider(0, 100, self.ros_gui_node.get_cur_finger_percent(self.oarbot_namespace), self.handle_finger_slider)
        layout.addWidget(self.finger_position_slider)

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

        super().resizeEvent(event)

    def handle_arm_home(self) -> None:
        pass

    def handle_base_home(self) -> None:
        pass

    def handle_arm_toggle_button(self, toggled: bool) -> None:
        self.arm_enabled = toggled
        self.arm_enable_button.setText("Arm Enabled" if toggled else "Arm Disabled")

    def handle_base_toggle_button(self, toggled: bool) -> None:
        self.base_enbled = toggled
        self.base_enable_button.setText("Base Enabled" if toggled else "Base Disabled")

    def handle_finger_slider(self, value: int) -> None:
        pass