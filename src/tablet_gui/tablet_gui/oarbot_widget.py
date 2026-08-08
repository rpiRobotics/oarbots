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
class OarbotWidget(QWidget):
    def __init__(self, oarbot_namespace: str, display_name: str, ros_gui_node: RosGuiNode) -> None:
        super().__init__()

        self.oarbot_namespace = oarbot_namespace
        self.display_name = display_name
        self.ros_gui_node = ros_gui_node

        layout = QVBoxLayout(self)
        layout.setAlignment(Qt.AlignmentFlag.AlignHCenter | Qt.AlignmentFlag.AlignTop)
        layout.setContentsMargins(24, 24, 24, 24)

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

        self.arm_enabled = False
        self.arm_enable_button = ToggleButton("Arm Enabled" if self.arm_enabled else "Arm Disabled", self.handle_arm_enable_button)
        layout.addWidget(self.arm_enable_button)

    def handle_arm_enable_button(self, checked: bool) -> None:
        self.arm_enabled = checked
        self.arm_enable_button.setText("Arm Enabled" if checked else "Arm Disabled")