from PySide6.QtCore import QTimer
from PySide6.QtWidgets import (
    QWidget,
    QHBoxLayout,
    QLabel
)
from PySide6.QtGui import QColor, QPainter, QFont
from PySide6.QtCore import Qt

from tablet_gui.ros_gui_node import RosGuiNode


class StatusIndicator(QWidget):
    def __init__(self, display_name: str, topic_node_name: str, ros_gui_node: RosGuiNode) -> None:
        super().__init__()

        self.display_name = display_name
        self.topic_node_name = topic_node_name
        self.ros_gui_node = ros_gui_node

        layout = QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)

        self.status_indicator = FilledCircle("#000000")
        layout.addWidget(self.status_indicator)
        self.node_name_text = QLabel(self.display_name)
        self.node_name_text.setFont(QFont("Arial", 16))
        layout.addWidget(self.node_name_text)

        layout.setAlignment(Qt.AlignmentFlag.AlignVCenter)

        # Create a timer for updating the status
        self.timer = QTimer(self)
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.update_status)
        self.timer.start()
        self.update_status()

    def update_status(self) -> None:
        if self.ros_gui_node.active_node_or_topic(self.topic_node_name):
            self.status_indicator.update_color("#00FF00")
        else:
            self.status_indicator.update_color("#FF0000")


class FilledCircle(QWidget):
    def __init__(self, color_hex: str) -> None:
        super().__init__()

        self.diameter = 20
        self.color = QColor(color_hex)

        self.setFixedSize(self.diameter, self.diameter)

    def paintEvent(self, event) -> None:
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        painter.setBrush(self.color)
        painter.setPen(Qt.PenStyle.NoPen)

        painter.drawEllipse(0, 0, self.diameter, self.diameter)

    def update_color(self, color_hex: str) -> None:
        self.color = QColor(color_hex)
        self.update()