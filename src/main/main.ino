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

}

void loop() {

  //As variáveis são atualizadas a cada loop
  getPosition(); // atualiza o vetor de coordenadas 
  temperature = getTemperature();
  pressure = getPressure();
  altitude = getAltitude();
  acx = getAcceleration('x'); acy = getAcceleration('y'); acz = getAcceleration('z');
  gx = getGyro('x'); gy = getGyro('y'); gz = getGyro('z');

  Serial.Print("Altitude (m): ") Serial.println(altitude);
  Serial.Print("Pressure (Pa): ") Serial.println(pressure);
  Serial.Print("Temperature (°C): ") Serial.println(temperature);
  Serial.Print("Latitude: ") Serial.Println(position[0], 6);
  Serial.Print("Longitude: ") Serial.Println(position[1], 6);

  delay(20);

}
