#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

unsigned long lastTime = 0;
bool falling;

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

void testMPURaw() {
  Serial.println("\n--- TESTE DIRETO MPU ---");

  Wire.beginTransmission(0x68);
  Wire.write(0x75);  // WHO_AM_I
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

void initMPU() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(1000);

  scanI2C();

  // ===== MPU ===== esse código foi adicionado pois o módulo MPU estava dando erro de inicialização
  resetMPU();
  testMPURaw();


  if (mpu.begin(0x68)) {
    Serial.println("MPU6050 Iniciado");
  } else {
    Serial.println("Módulo MPU6050 Não Encontrado");
  }
}

//em m/s
float getAcceleration(char axis){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); // Lê os dados atuais do sensor

  switch(axis) {
    case 'x': return a.acceleration.x;
    case 'y': return a.acceleration.y;
    case 'z': return a.acceleration.z;
    default: return 0.0;
  }
}

//em rad/s
float getGyro(char axis){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp); // Lê os dados atuais do sensor

  switch(axis){
    case 'x': return g.gyro.x;
    case 'y': return g.gyro.y;
    case 'z': return g.gyro.z;
    default: return 0.0;

  }
}

//ângulo em graus
float getAngle(char type){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //fórmula para cálculo de pitch (inclinação pra frente/trás)
  float pitch = atan2(a.acceleration.y, a.acceleration.z) * 57.2958;
  
  //fórmula para cálculo de roll (inclinação pra os lados)
  float roll = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 57.2958;

  if (type == 'p') return pitch;
  if (type == 'r') return roll;
  return 0.0;
}

bool isFalling(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float aceleracaoVertical = abs(a.acceleration.z);

  if(aceleracaoVertical > 1.96){
    lastTime = millis();
    falling = false;
  }else{
    if(millis() - lastTime > 500){
      falling = true;
    }
  }

  return falling;
}