from PySide6.QtGui import QCloseEvent
from PySide6.QtWidgets import QWidget
from tablet_gui.ros_gui_node import RosGuiNode
from threading import Event

class AppWindow(QWidget):
    def __init__(self, ros_gui_node: RosGuiNode, shutdown_event: Event) -> None:
        super().__init__()

        self.ros_gui_node = ros_gui_node
        self.shutdown_event = shutdown_event

    def closeEvent(self, event: QCloseEvent) -> None:
        self.shutdown_event.set()
        event.accept()