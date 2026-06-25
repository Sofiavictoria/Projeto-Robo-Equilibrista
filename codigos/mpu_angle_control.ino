// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h> //Biblioteca do MPU
#include <Adafruit_Sensor.h> // Biblioteca padrao de sensores
#include <Wire.h> // Biblioteca de comunicacao I2C
#include <math.h>


Adafruit_MPU6050 mpu; // Cria o objeto do MPU6050

// liga ou desliga os serial.prints para debugar
bool debug = false;


// variáveis do filtro complementar
float angle = 0.0;           // angulo após os filtros complementares
float alpha = 0.98;          // coeficiente alpha do filtro
unsigned long lastTime = 0; 

// variáveis para calibração
float gyroBiasX = 0.0, gyroBiasY = 0.0, gyroBiasZ = 0.0;
float accelBiasX = 0.0, accelBiasY = 0.0, accelBiasZ = 0.0;
float accelScaleX = 1.0, accelScaleY = 1.0, accelScaleZ = 1.0;

void calibrateSensors() {
  Serial.println("Calibrando. Não mova o sensor...");
  delay(2000);
  
  //fazendo uma média das primeiras 500 leituras dos sensores, para coletar o viés
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
  
  // viés do giroscópio
  gyroBiasY = sumGyroY / samples;
  
  // Viés do acelerômetro
  accelBiasX = (sumAccelX / samples) - 9.81; // devemos retirar G da coordenada apontando para cima
  accelBiasY = (sumAccelY / samples) - 0.0;
  accelBiasZ = (sumAccelZ / samples) - 0.0;
  if (!debug) {
    Serial.print("Gyro Y bias: ");
    Serial.print(gyroBiasY * 180/PI, 4);
    Serial.println(" deg/s");
    
    Serial.print("Accel bias X: ");
    Serial.print(accelBiasX, 4);
    Serial.print(" Y: ");
    Serial.print(accelBiasY, 4);
    Serial.print(" Z: ");
    Serial.println(accelBiasZ, 4);
  }
  
}

void setup(void) {
  // Velocidade serial mais baixa
  Serial.begin(115200);
  // Espera o Monitor Serial abrir
  while (!Serial) {  
    delay(10);
  }

  // Inicializa o MPU6050
  if (!mpu.begin()) {
    // Mostra erro caso nao encontre o sensor
    Serial.println("MPU6050 nao conectada");

    while (1) {
      delay(10);
    }
  }

  // Configuracoes do sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);  // Define alcance do acelerometro
  mpu.setGyroRange(MPU6050_RANGE_250_DEG); // Define alcance do giroscopio
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);  // Define filtro do sensor


  calibrateSensors();
  
  // Initialize with calibrated reading
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // registrando os primeiros valores do sensor, corrigidos pelo bias medido
  float ax = a.acceleration.x - accelBiasX;
  float az = a.acceleration.z - accelBiasZ;
  angle = atan2(az, ax) * 180.0 / PI; // angulo inicial parado

  lastTime = micros();
}

void loop() {
  // Leitura dos sensores
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate dt in seconds
  unsigned long currentTime = micros();
  float dt = (currentTime - lastTime) / 1000000.0;

  if (dt > 0.1 || dt <= 0) return;

  lastTime = currentTime;


  // Apply calibration
  float ax = a.acceleration.x - accelBiasX;
  float ay = a.acceleration.y - accelBiasY;
  float az = a.acceleration.z - accelBiasZ;

  float gy = (g.gyro.y - gyroBiasY) * 180.0 / PI;  // deg/s
  
  float accelAngle = atan2(az, ax) * 180.0 / PI;

  angle = alpha * (angle + gy * dt) + (1.0 - alpha) * accelAngle;


  // contador de print, para reduzir o impacto dos serial.prints nos atrasos do loop
  // esses prints serão retirados no programa final
  static unsigned long lastPrint = 0;
  if (!debug) {
    if (millis() - lastPrint > 50) {
      Serial.print(" Filtro:");
      Serial.print(angle, 2);
      Serial.print("Acelerômetro:");
      Serial.print(accelAngle, 2);
      Serial.print(" Giroscópio:");
      Serial.print(gy, 2);
      Serial.println();
      lastPrint = millis();
    }
  }


  // filtros complementares

  // implementar uma tara, pegando as primeiras leituras, tirando uma média e tarando neste valor

  // Mais devagar
  //delay(100);
}
