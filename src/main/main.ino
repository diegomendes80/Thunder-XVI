#include <Wire.h>
#include <SPI.h>
#include "BluetoothSerial.h" 


#define SDA 21
#define SCL 22
#define servoPin 4
#define csPin 5         // NSS pino do chip
#define resetPin 22     // RST
#define irqPin 26        // DIO0 pino de interrupção
#define SCK 18
#define MISO 19
#define MOSI 23

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
  Wire.begin(SDA, SCL);
  SPI.begin(SCK, MOSI, MISO, csPin);

  initBMP();
  initMPU();
  initGPS();
  initLoRa();
  initFlash();

  SerialBT.begin("Receptor_ESP32"); 
  Serial.println("Bluetooth iniciado! Já pode parear no celular.");


}

void loop() {

  //As variáveis são atualizadas a cada loop
  getPosition(position); // atualiza o vetor de coordenadas 
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();
  acx = getAcceleration('x'); 
  acy = getAcceleration('y'); 
  acz = getAcceleration('z');
  gx = getGyro('x'); 
  gy = getGyro('y'); 
  gz = getGyro('z');


  Serial.print("Altitude (m): "); Serial.println(altitude);
  Serial.print("Pressure (Pa): "); Serial.println(pressure);
  Serial.print("Temperature (°C): "); Serial.println(temperature);
  Serial.println();

  SerialBT.print("Altitude (m): "); SerialBT.println(altitude);
  SerialBT.print("Pressure (Pa): "); SerialBT.println(pressure);
  SerialBT.print("Temperature (°C): "); SerialBT.println(temperature);
  SerialBT.println();

  Serial.print("ax: "); Serial.println(acx);
  Serial.print("ay: "); Serial.println(acy);
  Serial.print("az: "); Serial.println(acz);
  Serial.println();

  SerialBT.print("ax: "); SerialBT.println(acx);
  SerialBT.print("ay: "); SerialBT.println(acy);
  SerialBT.print("az: "); SerialBT.println(acz);
  SerialBT.println();
  
  Serial.print("gx: "); Serial.println(gx);
  Serial.print("gy: "); Serial.println(gy);
  Serial.print("gz: "); Serial.println(gz);
  Serial.println();

  SerialBT.print("gx: "); SerialBT.println(gx);
  SerialBT.print("gy: "); SerialBT.println(gy);
  SerialBT.print("gz: "); SerialBT.println(gz);
  SerialBT.println();


  Serial.print("Latitude: "); Serial.println(position[0], 6);
  Serial.print("Longitude: "); Serial.println(position[1], 6);
  SerialBT.print("Latitude: "); SerialBT.println(position[0], 6);
  SerialBT.print("Longitude: "); SerialBT.println(position[1], 6);


  if(isFalling()){
    Serial.println("Caindo");
    SerialBT.println("Caindo");
  }


  delay(50);

}
