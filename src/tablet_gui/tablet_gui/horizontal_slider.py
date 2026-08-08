from PySide6.QtWidgets import QSlider
from PySide6.QtCore import Qt


class HorizontalSlider(QSlider):
    def __init__(self, min: int , max: int, initial_value: int, on_change):
        super().__init__(Qt.Orientation.Horizontal)

        self.setMinimum(min)
        self.setMaximum(max)
        self.setValue(initial_value)

        self.valueChanged.connect(on_change)

        self.setMinimumHeight(40)

        self.setStyleSheet("""
            QSlider {
                min-height: 40px;
            }

            QSlider::groove:horizontal {
                height: 12px;
                background: #cccccc;
                border-radius: 6px;
            }

            QSlider::handle:horizontal {
                width: 40;
                height: 40;
                margin: -8px 0;
                background: #5566EE;
                border-radius: 14px;
            }
        """)