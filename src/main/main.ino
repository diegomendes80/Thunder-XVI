#include <Wire.h>


#define SDA 21
#define SCL 22
#define servoPin 4

//VARIAVEIS ------:
float temperature, pressure, altitude;
double position[2];
float acx, acy, acz;
float gx, gy, gz;



void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);

  initBMP();
  initMPU();
  initGPS();
  initLoRa();

}

void loop() {

  //As variáveis são atualizadas a cada loop
  getPosition(position); // atualiza o vetor de coordenadas 
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();
  acx = getAcceleration('x'); acy = getAcceleration('y'); acz = getAcceleration('z');
  gx = getGyro('x'); gy = getGyro('y'); gz = getGyro('z');

  Serial.print("Altitude (m): "); Serial.println(altitude);
  Serial.print("Pressure (Pa): "); Serial.println(pressure);
  Serial.print("Temperature (°C): "); Serial.println(temperature);
  Serial.println();

  Serial.print("ax: "); Serial.println(acx);
  Serial.print("ay: "); Serial.println(acy);
  Serial.print("az: "); Serial.println(acz);
  Serial.println();
  
  Serial.print("gx: "); Serial.println(gx);
  Serial.print("gy: "); Serial.println(gy);
  Serial.print("gz: "); Serial.println(gz);




  if(isFalling()){
    Serial.println("Caindo");
  }

  // Serial.print("Latitude: "); Serial.println(position[0], 6);
  // Serial.print("Longitude: "); Serial.println(position[1], 6);

  delay(50);

}
