#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;

// ====== PINOS DOS MOTORES ======
#define M1_IN1 5
#define M1_IN2 6
#define M2_IN1 9
#define M2_IN2 10

// ====== FILTRO COMPLEMENTAR ======
float angle = 0.0;
float alpha = 0.98;
unsigned long lastTime = 0;

// ====== CALIBRAÇÃO ======
float gyroBiasY = 0.0;
float accelBiasX = 0.0, accelBiasY = 0.0, accelBiasZ = 0.0;

// ====== PID ======
float setPoint = 0.0;   // ângulo de equilíbrio
float Kp = 20.0;
float Ki = 0.0;
float Kd = 0.8;

float error = 0;
float lastError = 0;
float integral = 0;

// limite da velocidade PWM
int maxPWM = 255;
int minPWM = 35; // ajuda a vencer atrito do motor

bool debug = true;

// ====== FUNÇÕES DOS MOTORES ======

void motorWrite(int pin1, int pin2, int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    analogWrite(pin1, speed);
    digitalWrite(pin2, LOW);
  } 
  else if (speed < 0) {
    digitalWrite(pin1, LOW);
    analogWrite(pin2, -speed);
  } 
  else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }
}

void setMotors(int speed) {
  speed = constrain(speed, -255, 255);

  if (abs(speed) < minPWM) {
    speed = 0;
  }

  motorWrite(M1_IN1, M1_IN2, speed);
  motorWrite(M2_IN1, M2_IN2, speed);
}

void stopMotors() {
  setMotors(0);
}

// ====== CALIBRAÇÃO DA MPU ======

void calibrateSensors() {
  Serial.println("Calibrando... Não mova o sensor.");
  delay(2000);

  const int samples = 500;
  float sumGyroY = 0;
  float sumAccelX = 0;
  float sumAccelY = 0;
  float sumAccelZ = 0;

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sumGyroY += g.gyro.y;
    sumAccelX += a.acceleration.x;
    sumAccelY += a.acceleration.y;
    sumAccelZ += a.acceleration.z;

    delay(2);
  }

  gyroBiasY = sumGyroY / samples;

  accelBiasX = (sumAccelX / samples) - 9.81;
  accelBiasY = (sumAccelY / samples);
  accelBiasZ = (sumAccelZ / samples);

  Serial.println("Calibração finalizada.");
}

// ====== SETUP ======

void setup() {
  Serial.begin(115200);

  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);

  stopMotors();

  if (!mpu.begin()) {
    Serial.println("MPU6050 não conectada.");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrateSensors();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x - accelBiasX;
  float az = a.acceleration.z - accelBiasZ;

  angle = atan2(az, ax) * 180.0 / PI;

  // usa o ângulo inicial como ponto de equilíbrio
  setPoint = angle;

  lastTime = micros();

  Serial.print("SetPoint inicial: ");
  Serial.println(setPoint);
}

// ====== LOOP PRINCIPAL ======

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;

  if (dt <= 0 || dt > 0.1) {
    lastTime = currentTime;
    return;
  }

  lastTime = currentTime;

  // ====== LEITURA DO ÂNGULO ======
  float ax = a.acceleration.x - accelBiasX;
  float az = a.acceleration.z - accelBiasZ;

  float gy = (g.gyro.y - gyroBiasY) * 180.0 / PI;

  float accelAngle = atan2(az, ax) * 180.0 / PI;

  angle = alpha * (angle + gy * dt) + (1.0 - alpha) * accelAngle;

  // ====== SEGURANÇA ======
  // Se tombar demais, para os motores
  if (abs(angle - setPoint) > 35) {
    stopMotors();
    integral = 0;
    return;
  }

  // ====== PID ======
  error = angle - setPoint;

  integral += error * dt;
  integral = constrain(integral, -100, 100);

  float derivative = (error - lastError) / dt;

  float output = Kp * error + Ki * integral + Kd * derivative;

  lastError = error;

  int motorSpeed = constrain(output, -maxPWM, maxPWM);

  setMotors(motorSpeed);

  // ====== DEBUG ======
  if (debug) {
    static unsigned long lastPrint = 0;

    if (millis() - lastPrint > 50) {
      Serial.print("Angle: ");
      Serial.print(angle);

      Serial.print(" | SetPoint: ");
      Serial.print(setPoint);

      Serial.print(" | Error: ");
      Serial.print(error);

      Serial.print(" | PWM: ");
      Serial.println(motorSpeed);

      lastPrint = millis();
    }
  }
}
