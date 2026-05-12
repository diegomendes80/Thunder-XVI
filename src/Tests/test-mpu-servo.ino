#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h> // ✅ servo correto

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;
Servo servo;

// ===== SCAN I2C =====
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

// ===== TESTE MPU =====
void testMPURaw() {
  Serial.println("\n--- TESTE DIRETO MPU ---");

  Wire.beginTransmission(0x68);
  Wire.write(0x75);
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

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x80);
  Wire.endTransmission();
  delay(200);

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(200);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("=== TESTE COMPLETO MPU + BMP + SERVO ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(1000);

  scanI2C();

  // BMP
  Serial.println("\nTestando BMP280...");
  if (bmp.begin(0x76) || bmp.begin(0x77)) {
    Serial.println("BMP280 OK!");
  } else {
    Serial.println("BMP280 ERRO!");
  }

  // MPU
  resetMPU();
  testMPURaw();

  Serial.println("\nInicializando MPU (lib)...");
  if (mpu.begin(0x68)) {
    Serial.println("MPU6050 OK!");
  } else {
    Serial.println("MPU6050 FALHOU!");
  }

  // ===== SERVO =====
  servo.attach(4); // ⚠️ use 13 ou 27 (evita problema)
  servo.write(0);   // posição inicial
}


void loop() {
  Serial.println("\n--- LEITURAS ---");

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;

  Serial.print("Accel X: "); Serial.print(ax);
  Serial.print(" Y: ");       Serial.println(ay);

  float threshold = 1.5; // m/s² — ajuste aqui

  if (abs(ax) > threshold || abs(ay) > threshold) {
    servo.write(90);
    Serial.println("SERVO ATIVADO");
  } else {
    servo.write(0);
    Serial.println("SERVO NEUTRO");
  }

  delay(200);
} 