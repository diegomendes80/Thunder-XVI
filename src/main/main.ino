#include <Wire.h>
#include <SPI.h>
#include "BluetoothSerial.h"


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

//CONFIGURAÇÕES DO BLUETOOTH:
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;


//VARIAVEIS ------:
float temperature, pressure, altitude;
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

  // SerialBT.begin("Receptor_ESP32");
  Serial.println("Bluetooth iniciado! Já pode parear no celular.");
}

void loop() {

  //As variáveis são atualizadas a cada loop
  getPosition(position);  // atualiza o vetor de coordenadas
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();
  acx = getAcceleration('x');
  acy = getAcceleration('y');
  acz = getAcceleration('z');
  gx = getGyro('x');
  gy = getGyro('y');
  gz = getGyro('z');

  String telemetria = "====================================\n";
  telemetria += "          THUNDER-XVI - TELEMETRIA  \n";
  telemetria += "====================================\n";
  telemetria += "  [AMBIENTE]\n";
  telemetria += "  Altitude:    " + String(altitude, 2) + " m\n";
  telemetria += "  Pressao:     " + String(pressure, 2) + " Pa\n";
  telemetria += "  Temperatura: " + String(temperature, 1) + " C\n";
  telemetria += "------------------------------------\n";
  telemetria += "  [ACELEROMETRO] (m/s2)\n";
  telemetria += "  AX: " + String(acx, 2) + " | AY: " + String(acy, 2) + " | AZ: " + String(acz, 2) + "\n";
  telemetria += "------------------------------------\n";
  telemetria += "  [GIROSCOPIO] (rad/s)\n";
  telemetria += "  GX: " + String(gx, 2) + " | GY: " + String(gy, 2) + " | GZ: " + String(gz, 2) + "\n";
  telemetria += "------------------------------------\n";
  telemetria += "  [GPS]\n";
  telemetria += "  Latitude:  " + String(position[0], 6) + "\n";
  telemetria += "  Longitude: " + String(position[1], 6) + "\n";
  telemetria += "====================================\n\n";

  Serial.print(telemetria);
  // SerialBT.print(telemetria);
  sendData(telemetria);
  // if(isFalling()){
  //   Serial.println("Caindo");
  //   SerialBT.println("Caindo");
  // }


  delay(200);
}