from PySide6.QtGui import QCloseEvent
from PySide6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QVBoxLayout
from PySide6.QtCore import Qt
from threading import Event

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.oarbot_widget import OarbotWidget
from tablet_gui.oarbot_status import OarbotStatusItem

class MainWindow(QMainWindow):
    def __init__(self, ros_gui_node: RosGuiNode, shutdown_event: Event) -> None:
        super().__init__()

        self.ros_gui_node = ros_gui_node
        self.shutdown_event = shutdown_event

        self.setWindowTitle("OARBot GUI")

        # Set up base layout and add to the screen
        screen_widget = QWidget()
        screen_layout = QVBoxLayout()
        oarbot_widgets_layout = QHBoxLayout()
        oarbot_widgets_layout.addWidget(OarbotWidget("oarbot_blue", "OARBot Blue", self.ros_gui_node))
        oarbot_widgets_layout.addWidget(OarbotWidget("oarbot_silver", "OARBot Silver", self.ros_gui_node))
        screen_layout.addLayout(oarbot_widgets_layout)
        screen_layout.addStretch(1)

        nuc_status_layout = QHBoxLayout()
        nuc_status_layout.addStretch()
        nuc_status_layout.addWidget(OarbotStatusItem("Overhead NUC", "nuc", self.ros_gui_node))
        nuc_status_layout.addStretch()
        screen_layout.addLayout(nuc_status_layout)

        screen_layout.setAlignment(Qt.AlignmentFlag.AlignHCenter)
        screen_widget.setLayout(screen_layout)
        self.setCentralWidget(screen_widget)


    def closeEvent(self, event: QCloseEvent) -> None:
        self.shutdown_event.set()
        event.accept()