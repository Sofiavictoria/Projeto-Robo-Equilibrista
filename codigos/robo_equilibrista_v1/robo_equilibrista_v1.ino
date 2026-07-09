// Robô autoequilibrante - versão com Adafruit_MPU6050 + PID + L298N

#include <Adafruit_MPU6050.h> // Biblioteca do MPU
#include <Adafruit_Sensor.h>  // Biblioteca padrao de sensores
#include <Wire.h>             // Biblioteca de comunicacao I2C
#include <math.h>

Adafruit_MPU6050 mpu; // Objeto do MPU6050

// liga ou desliga os serial.prints para debugar
bool debug = false;

// ---------- Pinos do Driver L298N ----------
const int ENA = 5;   // PWM motor direito
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;  // PWM motor esquerdo

// variáveis do filtro complementar
float angle = 0.0;           // angulo apos o filtro complementar
float alpha = 0.98;          // coeficiente alpha do filtro
unsigned long lastTime = 0;

// variáveis para calibração
float gyroBiasY = 0.0;
float accelBiasX = 0.0, accelBiasY = 0.0, accelBiasZ = 0.0;

// ---------- PID ----------
// setpoint = angulo de equilibrio. Ajuste apos ver o valor de "angle"
// com o robo parado em pe (fisicamente apoiado) no Serial Monitor.
double setpoint = 0;

double erro = 0, erroAnterior = 0;
double integral = 0, derivada = 0;
double saidaPID = 0;

// >>> Estes 3 valores PRECISAM ser ajustados na pratica <<<
double Kp = 25;
double Ki = 140;
double Kd = 0.8;

void calibrateSensors() {
  Serial.println("Calibrando. Nao mova o sensor...");
  delay(2000);

  const int samples = 500;
  float sumGyroY = 0;
  float sumAccelX = 0, sumAccelY = 0, sumAccelZ = 0;

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

  accelBiasZ = (sumAccelZ / samples) + 9.81;
  accelBiasY = (sumAccelY / samples) - 0.0;
  accelBiasX = (sumAccelX / samples) - 0.0;

  if (!debug) {
    Serial.print("Gyro Y bias: ");
    Serial.print(gyroBiasY * 180 / PI, 4);
    Serial.println(" deg/s");

    Serial.print("Accel bias X: ");
    Serial.print(accelBiasX, 4);
    Serial.print(" Y: ");
    Serial.print(accelBiasY, 4);
    Serial.print(" Z: ");
    Serial.println(accelBiasZ, 4);
  }
}

void moverMotores(int velocidade) {
  velocidade = constrain(velocidade, -255, 255);

  if (velocidade >= 0) {
    // Frente
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } else {
    // Tras
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    velocidade = -velocidade;
  }
  analogWrite(ENA, velocidade);
  analogWrite(ENB, velocidade);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  if (!mpu.begin()) {
    Serial.println("MPU6050 nao conectada");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrateSensors();

  // Leitura inicial calibrada, para nao comecar o filtro do zero
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x - accelBiasX;
  float az = a.acceleration.z - accelBiasZ;
  angle = atan2(ax, -az) * 180.0 / PI;

  lastTime = micros();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;

  if (dt > 0.1 || dt <= 0) return;

  lastTime = currentTime;

  // Aplica calibracao
  float ax = a.acceleration.x - accelBiasX;
  float az = a.acceleration.z - accelBiasZ;

  float gy = (g.gyro.y - gyroBiasY) * 180.0 / PI; // deg/s

  float accelAngle = atan2(ax, -az) * 180.0 / PI;

  // Filtro complementar
  angle = alpha * (angle + gy * dt) + (1.0 - alpha) * accelAngle;

  // --- Controle PID ---
  erro = setpoint - angle;
  integral += erro * dt;
  integral = constrain(integral, -300, 300); // anti-windup

  derivada = (erro - erroAnterior) / dt;
  saidaPID = Kp * erro + Ki * integral + Kd * derivada;
  erroAnterior = erro;

  // Se o robo tombou demais, desliga os motores e zera o integral
  if (abs(angle) > 45) {
    moverMotores(0);
    integral = 0;
  } else {
    moverMotores((int)saidaPID);
  }

  // Debug a cada 50ms, sem travar o loop
  static unsigned long lastPrint = 0;
  if (!debug) {
    if (millis() - lastPrint > 50) {
      Serial.print("Filtro:");
      Serial.print(angle, 2);
      Serial.print(" PID:");
      Serial.print(saidaPID, 2);
      Serial.print(" Acelerometro:");
      Serial.print(accelAngle, 2);
      Serial.print(" Giroscopio:");
      Serial.print(gy, 2);
      Serial.println();
      lastPrint = millis();
    }
  }
}
