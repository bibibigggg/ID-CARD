from flask import Flask, request, jsonify    
import mysql.connector
from mysql.connector import Error
import ssl
import traceback
import logging
from gpiozero import Button, DigitalInputDevice, RGBLED, Servo, TonalBuzzer
from gpiozero.tones import Tone
from threading import Timer

class DeviceControl:
    def __init__ (self):

        # 하드웨어 초기화
        self.button = Button(15)                                                # 버튼
        self.led = RGBLED(red=17, green=27, blue=22)                            # RGB LED
        self.servo = Servo(18, min_pulse_width=0.00055, max_pulse_width=0.0023) # 자동문(서보모터)
        self.sensor = DigitalInputDevice(23)                                    # IR 센서
        self.buzzer = TonalBuzzer(24)                                           # 부저
        self.buzzer_frequency = 600                                             # 기본 부저 주파수 설정
        self.buzzer.stop()                                                      # 부저 초기 상태는 꺼짐                                                     
                                                          
    
        # 상태 변수
        self.emergency_stop = False  # 긴급 정지 상태 (초기 값: 비활성화)
        self.motor_running = False   # 모터 동작 여부 (초기 값: 비활성화)

        # 버튼 긴급 정지 콜백 설정
        self.button.when_pressed = self.handle_button_press  # 버튼 누름 시 호출

        # 초기 LED 상태 설정
        self.set_led_state("yellow")

    """
    자동문 상태에 따른 LED 설정
    EMS 정지: 빨간색
    대기: 노란색
    문 열림: 초록색
    """
    def set_led_state(self, state):

        if state == "red":
            self.led.color = (1, 0, 0)
            logging.info("[상태] 긴급 정지. (빨간색 불)")
        elif state == "yellow":
            self.led.color = (1, 1, 0)
            logging.info("[상태] 대기 중. (노란색 불)")
        elif state == "green":
            self.led.color = (0, 1, 0)
            logging.info("[상태] 문 열림. (초록색 불)")
        else:
            self.led.color = (0, 0, 0)  # 꺼짐

    """
    문 열기
    """
    def open_door(self):
        # 긴급 정지버튼이 눌렸을 때 혹은 모터가 작동한 상태가 아닐 때
        # moter_running = False
        if self.emergency_stop or self.motor_running: 
            return
        
        # 자동문을 열고 LED 상태 GREEN
        logging.info("문을 엽니다.")
        self.motor_running = True
        self.servo.value = 1  # 문 열기
        self.set_led_state("green")

        # 5초 후 문 닫기
        Timer(5, self.close_door).start()

    """
    문 닫기
    문 닫은 후 LED 상태 Yellow
    2초 후 모터 비활성화
    """
    def close_door(self):
        if self.emergency_stop:
            return

        logging.info("문을 닫습니다.")
        self.servo.value = -1  # 문 닫기
        self.set_led_state("yellow")

        # 2초 후 모터 비활성화
        Timer(2, self.stop_motor).start()

    """서보 모터 비활성화"""
    def stop_motor(self):
        logging.info("서보 모터 비활성화")
        self.motor_running = False
        self.cleanup()  # 하드웨어 초기화 및 재시작을 위한 초기화

    def check_sensor(self):
        """IR 센서 상태 확인"""
        if self.sensor.value == 0:
            logging.info("IR 센서: 물체 감지됨")
            return True
        logging.info("IR 센서: 물체 없음")
        return False
    
    # 버튼 눌림 이벤트 처리
    def handle_button_press(self):
        self.increase_buzzer_sound()  # 부저 소리 크게 만들기
        self.handle_emergency_stop()  # 긴급 정지 처리

    # 부저 소리 크게 만드는 함수 (300Hz ~ 900Hz 범위로 제한)
    def increase_buzzer_sound(self):
        if 300 <= self.buzzer_frequency <= 900:  # 범위 내에서만 설정
            logging.info(f"부저 주파수 설정: {self.buzzer_frequency}Hz")
            try:
                self.buzzer.play(Tone(frequency=self.buzzer_frequency))  # 부저 울림
                self.buzzer_frequency += 100  # 다음 소리를 위해 주파수 증가
                if self.buzzer_frequency > 900:  # 900Hz를 초과하면 다시 300Hz로 설정
                    self.buzzer_frequency = 300
            except ValueError as e:
                pass
        else:
            logging.error("부저 주파수가 허용 범위를 벗어났습니다.")
            
    def handle_emergency_stop(self):
        """긴급 정지 처리"""
        if not self.emergency_stop: 
            logging.info("긴급 정지 활성화")
            self.emergency_stop = True
            self.servo.value = -1  # 서보를 0도(닫힘) 위치로 설정
            self.set_led_state("red")  # 빨간불 켜기
            self.close_door()
            # 빨간불 깜박임 설정
            self.led.blink(on_time=0.5, off_time=0.5, on_color=(1, 0, 0), off_color=(0, 0, 0))

        else:  
            logging.info("긴급 정지 해제")
            self.emergency_stop = False
            self.set_led_state("yellow")
            self.open_door()  # 문 열기
            Timer(2, self.stop_motor).start()  # 모터 멈추기 타이머 설정

            # 부저 끄기
            self.buzzer.stop()  # 부저 끄기

    def process_door_control(self):
        while not self.emergency_stop:
            sensor_status = self.check_sensor() 
            if self.face_detected() and sensor_status:
                self.open_door()
                break

    def face_detected(self):
        """얼굴 인식 상태 확인"""
        # 얼굴 인식 로직 (임시로 True를 반환)
        # 실제 얼굴 인식 코드로 대체해야 함
        return True  # 얼굴 인식된 경우 True 반환
            
    def cleanup(self):
        self.button.close()
        self.servo.close()
        self.sensor.close()
        self.buzzer.close()  

app = Flask(__name__)

# 로깅 설정
logging.basicConfig(level=logging.DEBUG)

# 데이터베이스 연결 함수 (개선)
def get_db_connection():
    try:
        connection = mysql.connector.connect(
            host='3.36.108.189',  # AWS 서버 주소
            user='test',          # MySQL 사용자명
            password='1234',      # MySQL 비밀번호
            database='idcard'     # 데이터베이스 이름
        )
        if connection.is_connected():
            return connection
        
    except Error as e:
        logging.error(f"get_db_connection 오류: {e}")
        return None

@app.route('/verify_password', methods=['POST'])
def verify_password():
    try:
        if not request.is_json:
            return jsonify({'status': 'error', 'message': 'JSON 형식의 데이터가 필요합니다.'}), 400

        user_password = request.json.get('password')
        logging.debug(f"받은 비밀번호: {user_password}")  # 받은 비밀번호 로그

        if not user_password:
            return jsonify({'status': 'error', 'message': '비밀번호를 입력하세요'}), 400

        conn = get_db_connection()
        if conn is None:
            return jsonify({'status': 'error', 'message': '데이터베이스 연결 실패'}), 500

        cursor = conn.cursor()
        query = "SELECT passnumber, qr, name FROM idcard WHERE passnumber = %s"
        cursor.execute(query, (str(user_password),))
        result = cursor.fetchone()

        if result:
            qr_code_base64 = result[1]
            name = result[2]
            logging.debug(f"QR 코드 Base64: {qr_code_base64}")
            logging.debug(f"이름: {name}")

            if qr_code_base64:
                padding = len(qr_code_base64) % 4
                if padding != 0:
                    qr_code_base64 += '=' * (4 - padding)

                return jsonify({
                    'status': 'success',
                    'message': '비밀번호 일치',
                    'qr_code': qr_code_base64,
                    'name': name
                })
            else:
                return jsonify({'status': 'error', 'message': 'QR 코드 없음'}), 404
        else:
            return jsonify({'status': 'error', 'message': '잘못된 비밀번호'}), 401

    except Exception as e:
        logging.error(f"예상치 못한 오류: {str(e)}")
        traceback.print_exc()
        return jsonify({'status': 'error', 'message': '예상치 못한 오류가 발생했습니다.'}), 500

@app.route('/latest_passnumber', methods=['GET'])
def get_latest_passnumber():
    try:
        query = "SELECT passnumber FROM idcard ORDER BY id DESC LIMIT 1"

        # with 문을 사용하여 자동으로 연결 및 커서를 닫음
        with get_db_connection() as conn:
            with conn.cursor() as cursor:
                cursor.execute(query)
                result = cursor.fetchone()

        if result:
            passnumber = result[0]

            return jsonify({
                'status': 'success',
                'passnumber': passnumber
            }), 200
        else:
            return jsonify({'status': 'error', 'message': '데이터 없음'}), 404

    except Exception as e:
        logging.error(f"예상치 못한 오류: {str(e)}")
        traceback.print_exc()
        return jsonify({'status': 'error', 'message': '예상치 못한 오류가 발생했습니다.'}), 500

@app.route('/start_hard', methods=['POST'])
def start_hard():
    """문 열기/닫기 동작 시작"""
    try:
        if not request.is_json:
            return jsonify({'status': 'error', 'message': 'JSON 형식의 데이터가 필요합니다.'}), 400

        data = request.get_json()
        user_id = data.get('user_id')

        if not user_id:
            return jsonify({'status': 'error', 'message': 'user_id가 필요합니다.'}), 400

        # user_id가 'OK'일 경우 작업을 처리
        if user_id == "OK":
            logging.info("얼굴 인식 완료, 문 열림 시작")
            device = DeviceControl()
            
            while True:
                # 얼굴 인식 및 IR 센서 상태 확인
                sensor_status = device.check_sensor()
                if device.face_detected() and sensor_status:
                    logging.info("얼굴 인식 및 IR 센서 모두 감지됨, 문 열림 시작")
                    device.process_door_control()  # 내부에서 루프 종료
                    return jsonify({'status': 'success', 'message': '문이 열렸습니다.'}), 200

                # IR 센서 미감지 시 상태 로그
                if not sensor_status:
                    logging.warning("IR 센서 미감지, 대기 중...")
                    device.servo.value = -1  # 문 닫기
                
                # 얼굴 인식이 실패하면 루프 종료
                if not device.face_detected():
                    logging.warning("얼굴 인식 실패, 프로세스 중단")
                    return jsonify({'status': 'error', 'message': '얼굴 인식 실패'}), 400
        else:
            return jsonify({'status': 'error', 'message': '잘못된 user_id입니다.'}), 400
    except Exception as e:
        logging.error(f"오류 발생: {e}")
        return jsonify({'status': 'error', 'message': '서버 오류가 발생했습니다.'}), 500

if __name__ == '__main__':
    try:
        # 서버 시작 전에 사용자 데이터 로드

        # SSL 인증서와 키 파일을 사용해 HTTPS로 서버 실행
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(certfile="/home/pi/Project/venv/server.crt", keyfile="/home/pi/Project/venv/server.key")

        # 서버 실행
        app.run(host="0.0.0.0", port=5001, ssl_context=context, debug=True)

    except Exception as e:
        logging.error(f"서버 시작 오류: {e}")
        traceback.print_exc()  # 전체 스택 추적 출력
        exit(1)  # 서버 실행 실패 시 종료 