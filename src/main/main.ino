#define SDA 21
#define SCL 22

//VARIAVEIS ------:
float temperature, pressure, altitude;
float latitude, longitude;
float acx, acy, acz;
float gx, gy, gz;



void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);

  initBMP();

}

void loop() {

  //As variáveis são atualizadas a cada loop
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();


  Serial.Print("Altitude (m): ") Serial.println(altitude);
  Serial.Print("Pressure (Pa): ") Serial.println(pressure);
  Serial.Print("Temperature (°C): ") Serial.println(temperature);


  delay(20);

}
