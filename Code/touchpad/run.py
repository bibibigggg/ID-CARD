import sys
import cv2
import numpy as np
import mysql.connector
from PyQt5 import QtWidgets, QtGui, QtCore
from connection import Ui_win_connection
from qr import Ui_win_qr
import time
from PyQt5.QtWidgets import QApplication
import subprocess

# 데이터베이스 관리 클래스
class DatabaseManager:
    def __init__(self):
        try:
            # DB 정보 
            self.connection = mysql.connector.connect(
                host="3.36.108.189",
                port=3306,
                user="test",
                password="1234",
                database="idcard"
            )
            # DB 연결
            self.cursor = self.connection.cursor()
            # Table 체크(idcard가 있는가)
            self.check_table_exists()
        except mysql.connector.Error as err:
            print(f"데이터베이스 연결 오류: {err}")

    # Table 생성 유무 구분(에러를 잡기 위한 코드)
    # idcard Table이 있는지 확인인
    def check_table_exists(self):
        query = """
            SELECT COUNT(*) AS count
            FROM INFORMATION_SCHEMA.TABLES
            WHERE TABLE_SCHEMA = 'idcard' 
            AND TABLE_NAME = 'idcard';
        """
        self.cursor.execute(query)
        result = self.cursor.fetchone()
        if result[0] == 0:
            print("테이블이 존재하지 않습니다.")
        else:
            print("테이블이 존재합니다.")

    # 화면의 찍힌 QR에서 데이터를 뽑아 DB에 정보(주민번호)와 대조
    def get_qr_data(self, qr_data):
        print(f"Checking for QR data: {qr_data}")  # 디버깅용 출력
        query = "SELECT * FROM idcard WHERE pnumber = %s"
        self.cursor.execute(query, (qr_data,))
        result = self.cursor.fetchone()
        print(f"Query result: {result}")  # 디버깅용 출력
        return result is not None
    
    # 데이터 베이스와의 연결 종료
    def close(self):
        """커서와 연결을 안전하게 종료"""
        try:
            # 쿼리 결과를 모두 처리한 후 커서와 연결을 종료합니다.
            if self.cursor is not None:
                self.cursor.fetchall()  # 아직 처리되지 않은 결과가 있으면 모두 가져옵니다.
            # 데이터베이스 닫기
            self.cursor.close() 
            self.connection.close()
        except Exception as e:
            print(f"DB 연결 종료 오류: {e}")

# ConnectionWindow 클래스(PyQT 폼에 해당하는 화면들을 초기화 연결)
class ConnectionWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        # 처음 나오는 connection화면을 출력하기 위해 객체 생성
        self.ui = Ui_win_connection()
        self.ui.setupUi(self)
        # connection 내부 버튼을 누르면 open_qr_window로 연결
        self.ui.btn_QR.clicked.connect(self.open_qr_window)

    # 다음 QR인증 화면을 키는 함수
    def open_qr_window(self): 
        # QR 창 열기(QRWindow 객체 생성)
        self.qr_window = QRWindow()
        #터치패드에 화면크기에 맞추기 위한 FullScreen
        self.qr_window.showFullScreen()
        
        # 현재 창 닫기(해당 객체는 connetcthion클래스를 담당하고
        # open_qr_window를 하게되면 qr화면으로 변경되고 conection화면은 종료됨)
        self.close()

# OpenCV QR 스캐너 클래스
class OpenCVQRScanner(QtCore.QObject):
    frame_ready = QtCore.pyqtSignal(QtGui.QImage)  # 프레임 전송
    qr_detected = QtCore.pyqtSignal(str)  # QR 코드 데이터 전송
    error_occurred = QtCore.pyqtSignal(str)  # 오류 발생 시 전송

    def __init__(self):
        # 해당 코드들은 실시간 화면을 실행하기 위해 설정하는 코드들
        super().__init__()
        self.cap = None
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_frame) #지속적으로 새로운 화면 프레임을 보내기 위한 초기화
        
    def start(self):
        try:
            # OpenCV 실시간 화면 출력
            # DirectShow 백엔드 설정
            self.cap = cv2.VideoCapture(1, cv2.CAP_DSHOW) #해당 코드가 1인 이유 = "노트북 카메라 = 0번"
            if not self.cap.isOpened():
                raise IOError("카메라를 열 수 없습니다.")
            self.timer.start(50)  # 50ms 간격으로 프레임 업데이트
        except Exception as e:
            self.error_occurred.emit(str(e))

    def update_frame(self):
        try:
            # 프레임 읽기
            ret, frame = self.cap.read()
            if not ret:
                raise IOError("프레임을 읽을 수 없습니다.")

            # QR 코드 감지
            detector = cv2.QRCodeDetector() # OpenCV에 QR을 인식하는 기능 클래스
            # detectAndDecode = (data = 반환 변수, bbox = OR 좌표(QR의 디코딩해 내용을 반환하는 함수))
            data, bbox, _ = detector.detectAndDecode(frame)

            # QR 코드의 위치 표시(QR을 인식하면 Rect를 생성)
            if bbox is not None and len(bbox) > 0: 
                # bbox에 있는 좌표를 사용할 수 있도록 int형으로 변환 후 변수에 저장
                points = np.array(bbox, dtype=np.int32).reshape((-1, 2))
                # QR에 사각형 박스 그리기기
                cv2.polylines(frame, [points], isClosed=True, color=(91, 120, 107), thickness = 5)

            # 이미지를 RGB로 변환 후 QImage로 변환
            # QR 인증 화면에서 실시간 영상 출력을 위해 Image 데이터로 변환
            rgb_image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            h, w, ch = rgb_image.shape
            bytes_per_line = ch * w
            # PyQT에 맞는 iImage데이터로 변환하는 함수수
            convert_to_qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)

            # 프레임 전송
            self.frame_ready.emit(convert_to_qt_format)

            if data:
                self.qr_detected.emit(data)

        except Exception as e:
            self.error_occurred.emit(str(e))
            self.stop()

    def stop(self):
        """스캐너 정지 및 카메라 해제"""
        self.timer.stop()
        if self.cap and self.cap.isOpened():
            self.cap.release()

# QRWindow 클래스
class QRWindow(QtWidgets.QMainWindow, Ui_win_qr):
    def __init__(self):
        super().__init__()
        self.setupUi(self)

        # QGraphicsScene 객체를 생성하고 scene을 초기화
        # 해당 객체는 실시간 화면을 띄우기 위해 생성성
        self.scene = QtWidgets.QGraphicsScene(self)
        self.graphicsView.setScene(self.scene)  # graphicsView에 scene 설정

        # 인스턴스 생성 및 신호 연결
        self.scanner = OpenCVQRScanner()
        self.scanner.frame_ready.connect(self.update_frame)  # 프레임이 준비되면 처리
        self.scanner.qr_detected.connect(self.on_qr_detected)  # QR 코드 데이터가 감지되면 처리
        self.scanner.error_occurred.connect(self.on_error)  # 오류 처리

        # 데이터베이스 매니저 생성
        self.db_manager = DatabaseManager()
        self.current_qr_data = None
        self.detection_start_time = None  # QR 코드 감지 시간
        #해당 코드는 QR이 인식되고 바로 넘어간다면 자연스럽게 인식하고 넘어가는 것이 아니라
        #QR이 보이자 마자 넘어가서 사용자에게는 화면에 렉처럼 보이는 것을 지우기 위한 코드드
        self.time_threshold = 1.5  # 1.5초 후 QR 데이터 처리

        try:
            self.scanner.start()  # 스캐너 시작
        except Exception as e:
            print(f"Scanner 시작 오류: {e}")
            self.on_error(str(e))

    def log_qr_scanned(self, pnumber):
        """QR 코드 스캔 후 pnumber를 DB에 기록"""
        try:
            query = """
                INSERT INTO log (pnumber, access_time)
                VALUES (%s, NOW())
            """
            self.db_manager.cursor.execute(query, (pnumber,))
            self.db_manager.connection.commit()
            print(f"QR 스캔 기록 완료: {pnumber}")
        except mysql.connector.Error as err:
            print(f"DB 로그 기록 오류 (QR 스캔): {err}")
            self.db_manager.connection.rollback()

    def log_faceid_approved(self, pnumber):
        """FaceID 승인 후 승인된 시간을 DB에 기록"""
        try:
            query = """
                UPDATE log
                SET access_time = NOW()
                WHERE pnumber = %s AND access_time IS NULL
            """
            self.db_manager.cursor.execute(query, (pnumber,))

            # 쿼리 실행 후 결과를 모두 처리 (fetchall()을 호출하여 결과를 읽음)
            self.db_manager.cursor.fetchall()  # 결과를 처리해줍니다.

            self.db_manager.connection.commit()  # commit 호출
            print(f"FaceID 승인 기록 완료: {pnumber}")

        except mysql.connector.Error as err:
            # print(f"DB 로그 기록 오류 (FaceID 승인): {err}")
            self.db_manager.connection.rollback()  # 오류 발생 시 롤백

    def on_qr_detected(self, data):
        """QR 코드 감지 후 데이터 처리"""
        print(f"감지된 QR 코드: {data}")
        self.current_qr_data = data.strip()  # 데이터 저장

        # QR 코드가 처음 감지되었을 때 시간 기록
        if self.detection_start_time is None:
            # QR이 1.5초 이후에 인식하게 하기위한 time 함수수
            self.detection_start_time = time.time()

        # QR 코드가 일정 시간이 지난 후에만 처리
        if time.time() - self.detection_start_time >= self.time_threshold:
            #1.5초가 지난 이후 QR 데이터를 database에 데이터와 비교함함
            self.compare_with_database() # 258번째 줄 DB 함수 사용
        else:
            print("QR 코드가 감지되었으나, 처리 대기 중...")

    def update_frame(self, image):
        """프레임 업데이트: QGraphicsView에 이미지 표시"""
        try:
            pixmap = QtGui.QPixmap.fromImage(image)

            # QGraphicsView 크기 구하기
            view_width = self.graphicsView.width() # 실시간 화면의 넓이
            view_height = self.graphicsView.height() # 실시간 화면의 높이이

            # 이미지 크기를 QGraphicsView의 크기에 맞게 조정
            scaled_pixmap = pixmap.scaled(view_width, view_height, QtCore.Qt.KeepAspectRatio)

            # 기존 이미지 삭제 후 새로운 이미지 추가
            self.scene.clear()  
            self.scene.addPixmap(scaled_pixmap)

            # fitInView로 QGraphicsView의 크기에 맞추기
            self.graphicsView.setScene(self.scene)  # 화면에 scene을 설정
            self.graphicsView.fitInView(self.scene.sceneRect(), QtCore.Qt.KeepAspectRatio)

        except Exception as e:
            print(f"프레임 업데이트 오류: {e}")
            self.on_error(str(e))

    def on_error(self, error_message):
        """오류 처리"""
        QtWidgets.QMessageBox.critical(self, "오류", f"오류 발생: {error_message}")

    def compare_with_database(self):
        """QR 코드와 데이터베이스 비교"""
        if self.current_qr_data:
            try:
                pnumber = self.current_qr_data
                print(f"비교할 주민등록번호: {pnumber}")

                if self.db_manager.get_qr_data(pnumber): # 45번째 줄 QR데이터 추출 함수
                    self.scanner.stop()
                    self.open_faceid_window() # 얼굴 대조 화면 열기
                    self.close() # QR인증 화면 닫기
                else:
                    QtWidgets.QMessageBox.warning(self, "알림", "미등록 QR입니다.\nQR을 확인해주세요.")
                    self.quit_and_restart()
            except Exception as e:
                QtWidgets.QMessageBox.critical(self, "오류", f"비교 중 오류 발생: {str(e)}")
        else:
            QtWidgets.QMessageBox.warning(self, "알림", "QR이 인식되지 않았습니다.\n인식 영역을 확인해주세요.")

    def quit_and_restart(self):
        # 애플리케이션 종료 후 새로운 프로세스 실행
        QApplication.quit()  # 현재 애플리케이션 종료
        subprocess.Popen(["python", "run.py"])  # 새로운 프로세스 실행
        sys.exit()  # 애플리케이션 종료

    def open_faceid_window(self):
        """faceid.py 창을 실행하는 메서드"""
        from faceid import Ui_win_faceid
        
        msg_box = CustomMessageBox(self.centralwidget)
        msg_box.exec_()
        # faceid.py 창 열기
        self.faceid_window = QtWidgets.QMainWindow()
        self.faceid_ui = Ui_win_faceid()
        self.faceid_ui.setupUi(self.faceid_window)
        self.faceid_ui.set_pnumber(self.current_qr_data)  # 주민등록번호 전달
        self.faceid_window.showFullScreen()
        # faceid.py의 카메라 시작
        self.faceid_ui.start_camera()
        

    def closeEvent(self, event):
        # 창 닫힐 때 QR 스캔 & DB 연결 종료
        self.scanner.stop()
        self.db_manager.close()
        event.accept()

class CustomMessageBox(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        # 메시지 박스 스타일 설정
        self.setWindowTitle("알림")
        self.setWindowModality(QtCore.Qt.ApplicationModal)  # 모달 다이얼로그

        # 레이아웃 설정
        layout = QtWidgets.QVBoxLayout()

        # 텍스트 레이블 추가
        label = QtWidgets.QLabel("화면이 켜지면 얼굴을 가까이 해주세요.\n 얼굴 인식이 더 잘 되도록 해주세요.")
        label.setAlignment(QtCore.Qt.AlignCenter)
        layout.addWidget(label)

        # 텍스트 크기 스타일 적용
        label.setStyleSheet("font-size: 36px;")  # 텍스트 크기 설정
        layout.addWidget(label)

        # 버튼 추가
        ok_button = QtWidgets.QPushButton("확인")
        ok_button.setStyleSheet("font-size: 18px; padding: 15px 40px;")  # 버튼 텍스트 크기 및 패딩 설정
        ok_button.clicked.connect(self.accept)
        layout.addWidget(ok_button)

        # 레이아웃 설정
        self.setLayout(layout)

        # 크기 설정
        self.resize(700, 400)  # 원하는 크기로 설정
if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = ConnectionWindow()
    window.showFullScreen()
    sys.exit(app.exec_())
