from PySide6.QtWidgets import (
    QPushButton
)
from PySide6.QtGui import QColor, QPainter, QFont
from PySide6.QtCore import Qt

class PushButton(QPushButton):
    def __init__(self, text: str, on_click) -> None:
        super().__init__(text, self)

        self.setCheckable(False)
        self.clicked.connect(on_click)

        self.setFont(QFont("Roboto", 16))
        self.setStyleSheet("""
            QPushButton {
                padding: 16px 0px;
            }
        """)