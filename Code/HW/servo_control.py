import logging
from gpiozero import DigitalInputDevice

# IR 센서가 연결된 GPIO 핀 번호 설정
IR_SENSOR_PIN = 23  # 예시로 GPIO 17번 사용

class IR_Sensor:
    def __init__(self, pin):
        self.sensor = DigitalInputDevice(pin)

    def check_sensor(self):
        """IR 센서 상태 확인"""
        if self.sensor.value == 0:  # 물체 감지됨 (LOW)
            logging.info("IR 센서: 물체 감지됨")
            return True
        logging.info("IR 센서: 물체 없음")
        return False

# 로그 설정
logging.basicConfig(level=logging.INFO)

# IR 센서 객체 생성
ir_sensor = IR_Sensor(IR_SENSOR_PIN)

# IR 센서 상태 확인
if ir_sensor.check_sensor():
    print("물체가 감지되었습니다.")
else:
    print("물체가 감지되지 않았습니다.")
