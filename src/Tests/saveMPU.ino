#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

void scanI2C() {
  Serial.println("\n--- SCAN I2C ---");
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Dispositivo encontrado em 0x");
      Serial.println(i, HEX);
    }
  }
}

// ===== TESTE DIRETO MPU =====
void testMPURaw() {
  Serial.println("\n--- TESTE DIRETO MPU ---");

  Wire.beginTransmission(0x68);
  Wire.write(0x75); // WHO_AM_I
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 1);

  if (Wire.available()) {
    byte id = Wire.read();
    Serial.print("WHO_AM_I: 0x");
    Serial.println(id, HEX);
  } else {
    Serial.println("Falha ao ler WHO_AM_I");
  }
}

// ===== RESET MPU =====
void resetMPU() {
  Serial.println("\nResetando MPU...");

  // reset
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x80);
  Wire.endTransmission();
  delay(200);

  // wake up
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(200);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("=== TESTE COMPLETO MPU + BMP ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(1000);

  scanI2C();

  // ===== BMP =====
  Serial.println("\nTestando BMP280...");
  if (bmp.begin(0x76) || bmp.begin(0x77)) {
    Serial.println("BMP280 OK!");
  } else {
    Serial.println("BMP280 ERRO!");
  }

  // ===== MPU =====
  resetMPU();
  testMPURaw();

  Serial.println("\nInicializando MPU (lib)...");

  if (mpu.begin(0x68)) {
    Serial.println("MPU6050 OK!");
  } else {
    Serial.println("MPU6050 FALHOU!");
  }
}

void loop() {

  Serial.println("\n--- LEITURAS ---");

  // // BMP
  // Serial.print("Temp: ");
  // Serial.print(bmp.readTemperature());
  // Serial.print(" C | Pressao: ");
  // Serial.println(bmp.readPressure() / 100.0);

  // MPU
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accel X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" Z: ");
  Serial.println(a.acceleration.z);

  Serial.print("Gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print(" Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" Z: ");
  Serial.println(g.gyro.z);

  delay(100);
}