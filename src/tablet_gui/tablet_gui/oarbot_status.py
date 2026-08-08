from PySide6.QtCore import QTimer
from PySide6.QtWidgets import (
    QWidget,
    QGraphicsScene,
    QVBoxLayout,
    QHBoxLayout,
    QLabel
)
from PySide6.QtGui import QColor, QPainter, QFont
from PySide6.QtCore import Qt

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.status_indicator import StatusIndicator

class OarbotStatus(QWidget):
    def __init__(self, display_name_to_node_name: dict[str, str], ros_gui_node: RosGuiNode) -> None:
        super().__init__()

        self.display_name_to_node_name = display_name_to_node_name

        self.ros_gui_node = ros_gui_node

        # Graphics scene
        self.scene = QGraphicsScene(self)

        # Add each status item to the layout
        main_layout = QVBoxLayout(self)
        main_layout.setAlignment(Qt.AlignmentFlag.AlignHCenter)
        self.status_items: list[StatusIndicator] = []
        for display_name, node_name in display_name_to_node_name.items():
            cur_status_item = StatusIndicator(
                display_name=display_name,
                node_name=node_name,
                ros_gui_node=self.ros_gui_node
            )
            self.status_items.append(cur_status_item)
            main_layout.addWidget(cur_status_item)