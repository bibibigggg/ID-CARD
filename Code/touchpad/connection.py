from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication

class Ui_win_connection(object):
    def setupUi(self, win_connection):
        win_connection.setObjectName("win_connection")
        win_connection.resize(1920, 1280)
    
        font = QtGui.QFont()
        font.setFamily("Cafe24 Simplehae")
        font.setKerning(True)
        win_connection.setFont(font)
        win_connection.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))

        win_connection.setToolButtonStyle(QtCore.Qt.ToolButtonIconOnly)
        win_connection.setTabShape(QtWidgets.QTabWidget.Rounded)
        win_connection.setStyleSheet("background-color: #ffffff;")

        self.centralwidget = QtWidgets.QWidget(win_connection)
        self.centralwidget.setObjectName("centralwidget")
        self.btn_QR = QtWidgets.QPushButton(self.centralwidget)
        self.btn_QR.setGeometry(QtCore.QRect(660, 680, 600, 200))

        font = QtGui.QFont()
        font.setFamily("Cafe24 Simplehae")
        font.setPointSize(32)
        font.setKerning(True)
        self.btn_QR.setFont(font)        
        self.btn_QR.setObjectName("btn_QR")
        self.btn_QR.setStyleSheet("""font-weight: bold;
                                      color: white;
                                      background-color: #6B785B;""")

        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(460, 330, 1000, 200))

        font = QtGui.QFont()
        font.setFamily("Cafe24 Simplehae")
        font.setPointSize(48)
        font.setKerning(True)
        self.label.setFont(font)        
        self.label.setObjectName("label")

        win_connection.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(win_connection)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 21))
        self.menubar.setObjectName("menubar")
        win_connection.setMenuBar(self.menubar)

        self.retranslateUi(win_connection)
        QtCore.QMetaObject.connectSlotsByName(win_connection)

        # 두 번째 모니터로 이동
        self.move_to_secondary_monitor(win_connection)

    def retranslateUi(self, win_connection):
        _translate = QtCore.QCoreApplication.translate
        win_connection.setWindowTitle(_translate("win_connection", "Access sys"))
        self.btn_QR.setText(_translate("win_connection", "출입"))
        self.label.setText(_translate("win_connection", "<html><head/><body><p align=\"center\"><span style=\" font-size:48pt;\">IDNTT GUARD에 오신것을 환영합니다</span></p><p align=\"center\"><span style=\" font-size:36pt;\">출입을 위해 아래 버튼을 눌러주세요</span></p></body></html>"))

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
    win_connection = QtWidgets.QMainWindow()
    ui = Ui_win_connection()
    ui.setupUi(win_connection)
    win_connection.showFullScreen()
    sys.exit(app.exec_())
