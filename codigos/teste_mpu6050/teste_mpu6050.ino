// Programa: Teste MPU6050 sem LCD

#include <Wire.h>

// Endereco I2C do MPU6050
const int MPU = 0x68;

// Variaveis para armazenar valores dos sensores
int AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  // Inicializa o MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 iniciado");
}

void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  // Solicita os dados do sensor
  Wire.requestFrom(MPU, 14, true);

  // Leitura dos dados
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  Tmp = Wire.read() << 8 | Wire.read();

  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Exibe os dados no Monitor Serial
  Serial.print("AcX = ");
  Serial.print(AcX);

  Serial.print(" | AcY = ");
  Serial.print(AcY);

  Serial.print(" | AcZ = ");
  Serial.print(AcZ);

  Serial.print(" | Temp = ");
  Serial.print(Tmp / 340.00 + 36.53);

  Serial.print(" | GyX = ");
  Serial.print(GyX);

  Serial.print(" | GyY = ");
  Serial.print(GyY);

  Serial.print(" | GyZ = ");
  Serial.println(GyZ);

  delay(300);
}