#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// SDA => 21
// SCL => 22

Adafruit_BMP280 bmp;

//#define ALTITUDE_LOCAL 21 // Fortaleza

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializa I2C corretamente
  Wire.begin(21, 22);
  delay(100);

  Serial.println("Iniciando BMP280...");

  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("Erro ao encontrar o BMP280!");
    while (1) delay(10);
  }

  Serial.println("BMP280 iniciado com sucesso!");
}

void loop() {
  Serial.print("Temperatura = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" °C");

  Serial.print("Pressao = ");
  Serial.print(bmp.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Altitude estimada = ");
  Serial.print(bmp.readAltitude(1016.0)); // pressão ao nível do mar
  Serial.println(" m");

  Serial.println("-----------------------");
  delay(1000);
}