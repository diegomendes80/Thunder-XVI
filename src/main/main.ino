#include <Wire.h>
#include <SPI.h>
#include "BluetoothSerial.h"
#include <ESP32Servo.h>

#define SDA 21
#define SCL 22
#define servoPin 4
#define csPin 5      // NSS pino do chip
#define resetPin 14  // RST
#define irqPin 26    // DIO0 pino de interrupção
#define SCK 18
#define MISO 19
#define MOSI 23

void initMPU();
void initGPS();
void initLoRa();
void initBMP();  // Caso esteja em outro arquivo
void initFlash();
void getPosition(double v[2]);
float getAcceleration(char axis);
float getGyro(char axis);
float getTemperature();
float getPressure();
float getAltitude();
void logTelemetryToFlash(String telemetryPacket);
void dumpFlashTelemetry();
void eraseChipFull();


//VARIAVEIS ------:
float temperature = 0, pressure = 0, altitude = 0;
double position[2];
float acx, acy, acz;
float gx, gy, gz;


void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  Wire.begin(SDA, SCL);
  // Wire.setClock(100000);

  initBMP();
  initMPU();
  initGPS();
  initFlash();

  SPI.begin(SCK, MOSI, MISO, csPin);
  initLoRa();

  // servo.attach(4);

  // ATENÇÃO - OPÇÕES DE CONTROLE DA MEMÓRIA FLASH ======================================
  // dumpFlashTelemetry();  //DESCOMENTE PARA LER O QUE JÁ ESTÁ GRAVADO
  // eraseChipFull(); // DESCOMENTE PARA APAGAR TODA A MEMÓRIA
}

void loop() {
  getPosition(position);
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();
  acx = getAcceleration('x');
  acy = getAcceleration('y');
  acz = getAcceleration('z');
  gx = getGyro('x');
  gy = getGyro('y');
  gz = getGyro('z');

  // Criamos uma string ultra compacta para o rádio aceitar (Ex: "8.01,101503.5,34.0...")
  String dadosCompactos = String(altitude, 2) + "," + String(pressure, 2) + "," + String(temperature, 1) + ","
                        + String(acx, 2) + "," + String(acy, 2) + "," + String(acz, 2) + ","
                        + String(gx, 2) + "," + String(gy, 2) + "," + String(gz, 2) + ","
                        + String(position[0], 6) + "," + String(position[1], 6);

  // No Serial Monitor do TX, você ainda pode ver o texto completo se quiser:
  Serial.println("Enviando via LoRa (Compactado): " + dadosCompactos);

  logTelemetryToFlash(dadosCompactos); //DESCOMENTE PARA HABILITAR A GRAVAÇÃO NA MEMÓRIA 

  // Envia os dados leves pelo rádio
  sendData(dadosCompactos);

  delay(1000);
}