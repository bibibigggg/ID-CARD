import cv2
import mediapipe as mp
import numpy as np
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtCore import QTimer
import mysql.connector
from mysql.connector import Error
from PIL import Image
import io
from scipy.spatial.distance import euclidean
import requests
import time
from datetime import datetime
from scipy.spatial.distance import cosine
import subprocess
import sys
from PyQt5.QtWidgets import QApplication


def quit_and_restart():
    # 애플리케이션 종료 후 새로운 프로세스 실행
    QApplication.quit()  # 현재 애플리케이션 종료
    subprocess.Popen(["python", "run.py"])  # 새로운 프로세스 실행
    sys.exit()  # 애플리케이션 종료


class Ui_win_faceid(object):
    def __init__(self):
        self.start_time = None  # 얼굴 인식 시작 시간을 추적
        self.is_face_detected = False  # 얼굴이 인식되었는지 여부
        self.face_detection_duration = 1  # 얼굴 인식 지속 시간 (초)
        # 클래스 초기화 및 Mediapipe 모듈과 타이머 설정
        self.mp_face_detection = mp.solutions.face_detection  # Mediapipe 얼굴 감지 모듈
        self.mp_face_mesh = mp.solutions.face_mesh  # Mediapipe 얼굴 랜드마크 모듈
        self.face_detection = self.mp_face_detection.FaceDetection(min_detection_confidence=0.2)
        self.face_mesh = self.mp_face_mesh.FaceMesh(min_detection_confidence=0.2, min_tracking_confidence=0.5)
        self.timer = QTimer()  # PyQt5 타이머 생성
        self.pnumber = None  # QR로 전달받은 주민등록번호 저장

    def setupUi(self, win_faceid):
        # PyQt5를 이용한 UI 설정
        win_faceid.setObjectName("win_faceid")  # 윈도우 이름
        win_faceid.resize(1920, 1280)  # 윈도우 크기
        # 윈도우 크기 고정
        win_faceid.setStyleSheet("background-color: #ffffff;")

        # 글꼴 설정
        font = QtGui.QFont()
        font.setFamily("Cafe24 Simplehae")
        win_faceid.setFont(font)

        # 중앙 위젯 및 얼굴 표시 뷰 설정
        self.centralwidget = QtWidgets.QWidget(win_faceid)
        self.centralwidget.setObjectName("centralwidget")
        self.graphicsView = QtWidgets.QGraphicsView(self.centralwidget)  # 윈도우 센터에 실시간 화면
        self.graphicsView.setGeometry(QtCore.QRect(20, 20, 1875, 1235))  # 실시간 화면 크기 설정
        self.graphicsView.setObjectName("graphicsView")

        win_faceid.setCentralWidget(self.centralwidget)  # 메인 윈도우에 중앙 위젯 추가

        self.retranslateUi(win_faceid)
        QtCore.QMetaObject.connectSlotsByName(win_faceid)
        self.move_to_secondary_monitor(win_faceid)

    def move_to_secondary_monitor(self, win_connection):
        # 현재 시스템에 연결된 모니터 정보 가져오기
        screen_count = QApplication.screens()

        if len(screen_count) > 1:  # 두 번째 모니터가 있다면
            second_screen = screen_count[1]  # 두 번째 모니터
            screen_geometry = second_screen.geometry()  # 두 번째 모니터의 화면 크기
            win_connection.move(screen_geometry.topLeft())  # 두 번째 모니터의 좌측 상단에 창 위치

    def retranslateUi(self, win_faceid):
        # UI 텍스트 설정
        _translate = QtCore.QCoreApplication.translate
        win_faceid.setWindowTitle(_translate("win_faceid", "FaceID sys"))  # 윈도우 제목 설정

    def set_pnumber(self, pnumber):
        # QR로 전달받은 주민등록번호 설정
        self.pnumber = pnumber

    def start_camera(self):
        # 카메라를 시작하고 프레임 업데이트 타이머를 실행행
        self.cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)  # 카메라 초기화, cv2.CAP_DSHOW 는 OS에 따라 변경해줘야 함.
        self.timer.timeout.connect(self.update_frame)  # 타이머가 만료될 때마다 update_frame
        self.timer.start(50)  # 30ms 간격으로 프레임을 업데이트

    def update_frame(self):
        # 프레임을 읽어 얼굴을 인식하고 화면에 표시
        ret, frame = self.cap.read()
        if not ret:
            print("카메라에서 프레임을 읽을 수 없습니다.")
            return

        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # BGR에서 RGB로 변환
        results = self.face_mesh.process(frame_rgb)  # Mediapipe를 이용한 얼굴 랜드마크 추출

        # 얼굴 랜드마크가 존재하면 얼굴이 감지된 것으로 간주
        if results.multi_face_landmarks:
            for face_landmarks in results.multi_face_landmarks:
                self.draw_landmarks(frame, face_landmarks)  # 랜드마크 그리기

                if not self.is_face_detected:
                    self.start_time = time.time()  # 얼굴 인식 시작 시간 기록
                    self.is_face_detected = True  # 얼굴이 감지되었음을 표시
                else:
                    # 얼굴 인식 지속 시간 확인
                    elapsed_time = time.time() - self.start_time
                    if elapsed_time >= self.face_detection_duration:
                        self.compare_faces(frame, face_landmarks)  # 1초 이상 얼굴이 인식되면 비교 수행
                    if elapsed_time >= 5:
                        msg_box = CustomMessageBox2(self.centralwidget)
                        msg_box.exec_()
                        quit_and_restart()
        else:
            # 얼굴이 감지되지 않으면 상태 초기화
            self.is_face_detected = False

        # PyQt5에서 사용할 수 있도록 이미지를 변환
        rgb_image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        h, w, ch = rgb_image.shape
        bytes_per_line = ch * w
        convert_to_qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
        pixmap = QtGui.QPixmap.fromImage(convert_to_qt_format)

        # QGraphicsView에 변환된 이미지를 표시
        self.graphicsView.setScene(QtWidgets.QGraphicsScene(self.centralwidget))  # self.centralwidget로 수정
        self.graphicsView.scene().clear()
        self.graphicsView.scene().addPixmap(pixmap)
        self.graphicsView.fitInView(self.graphicsView.scene().sceneRect(), QtCore.Qt.KeepAspectRatio)

    def draw_landmarks(self, frame, landmarks):
        # 얼굴 랜드마크를 그리는 메서드
        for landmark in landmarks.landmark:
            x, y = int(landmark.x * frame.shape[1]), int(landmark.y * frame.shape[0])
            cv2.circle(frame, (x, y), 2, (0, 255, 0), -1)

    def capture(self):
        # 현재 화면을 캡처
        ret, frame = self.cap.read()
        quality = 50
        if not ret:
            print("프레임 캡처 실패")
            return

        # 이미지를 메모리에 저장
        encode_param = [cv2.IMWRITE_JPEG_QUALITY, quality]  # JPEG 압축 품질 설정
        _, buffer = cv2.imencode('.jpg', frame, encode_param)  # JPEG 형식으로 인코딩
        blob_data = buffer.tobytes()  # Blob 형식으로 변환
        return blob_data

    def compare_faces(self, frame, face_landmarks):
        # DB의 얼굴 데이터와 현재 얼굴 데이터 비교
        if self.pnumber:
            try:
                # DB 관리자 초기화
                db = DatabaseManager()
                face_data = db.get_face_data(self.pnumber)  # 주민등록번호로 얼굴 데이터 검색

                # BLOB 데이터를 이미지로 변환
                image = Image.open(io.BytesIO(face_data))
                np_image = np.array(image)

                # 학습된 얼굴의 랜드마크 추출
                face_landmarks_db = self.extract_face_landmarks(np_image)

                # 얼굴 랜드마크 유사도 계산 & 비교: Euclidean distance로 계산
                similarity = self.calculate_landmark_similarity(face_landmarks, face_landmarks_db)

                # 유사도 출력 (임계값을 설정하여 인증을 처리할 수 있음)
                print(f"얼굴 유사도: {similarity}")
                
                # 유사도에 따라 처리
                if similarity > 91:
                    msg_box = CustomMessageBox1(self.centralwidget)
                    msg_box.exec_()
                    face_data = self.capture()
                    if face_data is not None: 
                        print("얼굴사진이 존재합니다.")
                        db.log_access_time(self.pnumber, face_data)
                    else:    
                        print("blob 데이터 없음")

                    user_id = "OK"

                    # Flask 서버에 데이터 전송
                    flask_url = "https://192.168.0.220:5001/start_hard"
                    payload = {"user_id": user_id}

                    beet_time = time.time()
                    
                    try:
                        response = requests.post(flask_url, json=payload, verify=False)
                        
                        if response.status_code == 200:
                           #QtWidgets.QMessageBox.information(None, "알림", "서버 통신 성공")
                            quit_and_restart()
                        else:
                            QtWidgets.QMessageBox.warning(None, "알림", f"서버 응답 오류: {response.status_code}")
                    except requests.RequestException as e:
                        QtWidgets.QMessageBox.critical(None, "알림", f"서버와 통신 실패: {str(e)}")

                    quit_and_restart()
                #elif similarity < 50 and similarity > 20:
                    #QtWidgets.QMessageBox.warning(self.centralwidget, "알림", "얼굴을 조금 더 가까이 해주세요.")

            except ValueError as e:
                print(f"얼굴 데이터 오류: {e}")

    def extract_face_landmarks(self, image):
        # 이미지에서 얼굴 랜드마크 추출
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        results = self.face_mesh.process(image_rgb)
        
        if results.multi_face_landmarks:
            landmarks = results.multi_face_landmarks[0]  # 첫 번째 얼굴 랜드마크만 추출 (여러 얼굴에 대해서도 확장 가능)
            return landmarks
        else:
            raise ValueError("얼굴 랜드마크를 찾을 수 없습니다.")

    def calculate_landmark_similarity(self, detected_landmarks, db_landmarks):
        # 얼굴 랜드마크 간의 유사도를 계산
        detected_coords = [(landmark.x, landmark.y) for landmark in detected_landmarks.landmark]
        db_coords = [(landmark.x, landmark.y) for landmark in db_landmarks.landmark]
        
        # 유클리드 거리 계산: 각 랜드마크 간의 차이
        distances = []
        for detected, db in zip(detected_coords, db_coords):
            distance = np.linalg.norm(np.array(detected) - np.array(db))  # 유클리드 거리 계산
            distances.append(distance)
        
        # 전체 랜드마크에 대해 평균 거리를 계산하여 유사도 점수로 사용
        similarity_score = np.mean(distances)
        # 거리가 작을수록 얼굴이 유사하므로 유사도는 거리의 역수로 계산
        return 1 / (1 + similarity_score) * 100  # 0~100 사이의 유사도 점수 반환

    def stop(self):
        # 카메라 중지 및 리소스 해제제
        self.timer.stop()
        self.cap.release()

    def closeEvent(self, event):
        # 윈도우 종료 시 카메라 리소스 정리
        self.stop()
        event.accept()


class DatabaseManager:
    def __init__(self):
        # DB 연결 초기화
        self.connection = mysql.connector.connect(
            host="3.36.108.189",
            port=3306,
            user="test",
            password="1234",
            database="idcard",
            buffered=True  # buffered=True로 설정
        )
        self.cursor = self.connection.cursor()
        self.connection.autocommit = True  # autocommit 활성화
        self.face_cache = {}

    def get_face_data(self, pnumber):
        # 캐시에서 얼굴 데이터 확인
        if pnumber in self.face_cache:
            return self.face_cache[pnumber]

        # DB에서 얼굴 데이터를 가져옴
        query = "SELECT face FROM idcard WHERE pnumber = %s"
        self.cursor.execute(query, (pnumber,))
        result = self.cursor.fetchone()

        if result and result[0]:
            face_data = result[0]  # BLOB 데이터 그대로 반환
            self.face_cache[pnumber] = face_data
            return face_data
        else:
            raise ValueError("유효한 얼굴 데이터를 찾을 수 없습니다.")

    def log_access_time(self, pnumber, face_data):
        try:
            print("DB 로그 기록을 시도합니다.")
            
            # idcard 테이블에서 이름 가져오기
            name_query = "SELECT name FROM idcard WHERE pnumber = %s"
            self.cursor.execute(name_query, (pnumber,))
            
            # 결과를 fetchone()으로 읽어옵니다
            name_result = self.cursor.fetchone()
            
            if name_result:
                name = name_result[0]
            else:
                name = "Unknown"  # 이름을 찾을 수 없을 경우 기본값 설정
                print(f"이름을 찾을 수 없습니다. pnumber: {pnumber}")

            # 이제 두 번째 쿼리를 실행 (INSERT 쿼리)
            now = datetime.now()
            query = """
                INSERT INTO log (name, pnumber, access_time, face_data)
                VALUES (%s, %s, %s, %s)
            """
            self.cursor.execute(query, (name, pnumber, now, face_data))

            # commit을 수동으로 호출하여 DB 변경 사항을 저장
            self.connection.commit()
            print(f"{pnumber}에 대한 출입 로그가 기록되었습니다.")

        except mysql.connector.Error as err:
            print(f"DB 로그 기록 오류: {err}")
        except Exception as e:
            print(f"기타 오류 발생: {e}")

    def close(self):
        # DB 연결 종료
        try:
            self.connection.close()
        except Exception as e:
            print(f"종료 중 오류 발생: {e}")

class CustomMessageBox1(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        # 메시지 박스 스타일 설정
        self.setWindowTitle("알림")
        self.setWindowModality(QtCore.Qt.ApplicationModal)  # 모달 다이얼로그

        # 레이아웃 설정
        layout = QtWidgets.QVBoxLayout()

        # 텍스트 레이블 추가
        label = QtWidgets.QLabel("얼굴이 인증되었습니다.")
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

class CustomMessageBox2(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)

        # 메시지 박스 스타일 설정
        self.setWindowTitle("알림")
        self.setWindowModality(QtCore.Qt.ApplicationModal)  # 모달 다이얼로그

        # 레이아웃 설정
        layout = QtWidgets.QVBoxLayout()

        # 텍스트 레이블 추가
        label = QtWidgets.QLabel("얼굴 인증에 실패했습니다.")
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
