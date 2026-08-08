from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLabel
)
from PySide6.QtGui import QFont
from PySide6.QtCore import Qt

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.oarbot_status import OarbotStatus
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