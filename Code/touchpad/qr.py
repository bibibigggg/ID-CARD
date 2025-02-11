from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication

class Ui_win_qr(object):
    def setupUi(self, win_qr):
        win_qr.setObjectName("win_qr")
        win_qr.resize(1920, 1280)
        win_qr.setStyleSheet("background-color: #ffffff;")

        font = QtGui.QFont()
        font.setFamily("Cafe24 Simplehae")

        self.centralwidget = QtWidgets.QWidget(win_qr)
        self.centralwidget.setObjectName("centralwidget")
        self.graphicsView = QtWidgets.QGraphicsView(self.centralwidget)
        self.graphicsView.setGeometry(QtCore.QRect(20, 20, 1875, 1235))
        self.graphicsView.setObjectName("graphicsView")

        win_qr.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(win_qr)
        self.statusbar.setObjectName("statusbar")
        win_qr.setStatusBar(self.statusbar)

        self.retranslateUi(win_qr)
        QtCore.QMetaObject.connectSlotsByName(win_qr)
        
        # 두 번째 모니터로 이동
        self.move_to_secondary_monitor(win_qr)

    def retranslateUi(self, win_qr):
        _translate = QtCore.QCoreApplication.translate
        win_qr.setWindowTitle(_translate("win_qr", "QRCode sys"))

    def move_to_secondary_monitor(self, win_connection):
        # 현재 시스템에 연결된 모니터 정보 가져오기
        screen_count = QApplication.screens()

        if len(screen_count) > 1:  # 두 번째 모니터가 있다면
            second_screen = screen_count[1]  # 두 번째 모니터
            screen_geometry = second_screen.geometry()  # 두 번째 모니터의 화면 크기
            win_connection.move(screen_geometry.topLeft())  # 두 번째 모니터의 좌측 상단에 창 위치

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    win_qr = QtWidgets.QMainWindow()
    ui = Ui_win_qr()
    ui.setupUi(win_qr)
    win_qr.show()
    sys.exit(app.exec_())