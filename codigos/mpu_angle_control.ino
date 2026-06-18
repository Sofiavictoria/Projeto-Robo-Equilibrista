// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h> //Biblioteca do MPU
#include <Adafruit_Sensor.h> // Biblioteca padrao de sensores
#include <Wire.h> // Biblioteca de comunicacao I2C
#include <math.h>


Adafruit_MPU6050 mpu; // Cria o objeto do MPU6050

void setup(void) {

  // Velocidade serial mais baixa
  Serial.begin(9600);
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
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);  // Define alcance do acelerometro
  mpu.setGyroRange(MPU6050_RANGE_250_DEG); // Define alcance do giroscopio
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);  // Define filtro do sensor

  Serial.println("MPU6050 iniciado");
  delay(1000);
}

void loop() {
  float angle;
  // Leitura dos sensores
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Acelerometro
  //Serial.println("=== ACELEROMETRO ===");

  //Serial.print("X: "); // Mostra eixo X do acelerometro
  //Serial.println(a.acceleration.x);

  //Serial.print("Y: ");// Mostra eixo Y do acelerometro
  //Serial.println(a.acceleration.y);

  //Serial.print("Z: "); // Mostra eixo Z do acelerometro
  //Serial.println(a.acceleration.z);

  angle = atan((a.acceleration.z)/(a.acceleration.x)); //calculando o ângulo usando o math.h
  angle = angle * (180/M_PI);
  Serial.print("Ângulo: "); 
  Serial.println(angle);


  // Giroscopio
  //Serial.println("=== GIROSCOPIO ===");
  
  //Serial.print("X: "); // Mostra eixo X do giroscopio
  //Serial.println(g.gyro.x);

  //Serial.print("Y: "); // Mostra eixo Y do giroscopio
  //Serial.println(g.gyro.y);

  //Serial.print("Z: "); // Mostra eixo Z do giroscopio
  //Serial.println(g.gyro.z);

  // Temperatura do sensor
  //Serial.print("Temperatura: ");
  //Serial.println(temp.temperature);

  //Serial.println("------------------------");

  // Mais devagar
  delay(500);
}