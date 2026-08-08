from PySide6.QtWidgets import (
    QPushButton
)
from PySide6.QtGui import QColor, QPainter, QFont, QPalette
from PySide6.QtCore import Qt

class ToggleButton(QPushButton):
    def __init__(self, text: str, on_click, font_size=16) -> None:
        super().__init__(text)

        self.setCheckable(True)
        self.clicked.connect(on_click)

        # Handle color changes for clicking the button; we want a more pronounced color change than the default
        self.default_color = self.palette().button()
        self.clicked.connect(self.update_color)

        self.setFont(QFont("Roboto", font_size))
        self.setStyleSheet("""
            QPushButton {
                padding: 16px 0px;
            }
        """)

    def update_color(self, toggled: bool) -> None:
        palette = self.palette()

        if toggled:
            palette.setColor(QPalette.ColorRole.Button, QColor("#5566EE"))
        else:
            palette.setBrush(QPalette.ColorRole.Button, self.default_color)

        self.setPalette(palette)