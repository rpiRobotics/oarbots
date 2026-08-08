from PySide6.QtGui import QCloseEvent
from PySide6.QtWidgets import QMainWindow, QWidget, QHBoxLayout
from threading import Event

from tablet_gui.ros_gui_node import RosGuiNode
from tablet_gui.oarbot_widget import OarbotWidget

class MainWindow(QMainWindow):
    def __init__(self, ros_gui_node: RosGuiNode, shutdown_event: Event) -> None:
        super().__init__()

        self.ros_gui_node = ros_gui_node
        self.shutdown_event = shutdown_event

        self.setWindowTitle("OARBot GUI")

        # Set up base layout and add to the screen
        screen_widget = QWidget()
        screen_layout = QHBoxLayout()
        screen_layout.addWidget(OarbotWidget("oarbot_blue", "OARBot Blue", self.ros_gui_node))
        screen_layout.addWidget(OarbotWidget("oarbot_silver", "OARBot Silver", self.ros_gui_node))
        screen_widget.setLayout(screen_layout)

        self.setCentralWidget(screen_widget)


    def closeEvent(self, event: QCloseEvent) -> None:
        self.shutdown_event.set()
        event.accept()