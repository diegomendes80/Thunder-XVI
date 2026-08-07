#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

unsigned long lastTime = 0;
bool falling;

Adafruit_MPU6050 mpu; 

// Varre todas as portas lógicas de comunicação para ver se há algum dispositivo conectado
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

// Faz o reconhecimento do MPU6050 para saber se há comunicação
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

// Esse comando reseta o MPU6050 para que ele comece a medir.
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

// Esse comando tem como função, iniciar o escaneamento das portas lógicas, resetar o sensor, testar o chip, 
// e inicar a biblioteca do MPU para a leitura dos dados. 
void initMPU() {

  delay(2000);

  // Wire.begin(SDA, SCL);

  // ===== MPU ===== esse código foi adicionado pois o módulo MPU estava dando erro de inicialização
  scanI2C();
  resetMPU();
  testMPURaw();


  if (mpu.begin(0x68, &Wire)) {
    Serial.println("MPU6050 Iniciado");
  } else {
    Serial.println("Módulo MPU6050 Não Encontrado");
  }
}

// Retorna a aceleração em metros por segundo ao quadrado (m/s²). 
// Ele lê os eixos informados('x', 'y' ou 'z') e informa o valor correspondente em m/s².
// O comando mpu.getEvent(&a, &g, &temp), lê os dados atuais do sensor, aceleração atual (a), rotação (g) e temperatura (temp).
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

// informa a velocidade de rotação do giroscópio em radianos por segundo (rad/s).
float getGyro(char axis){
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

// CALCULO DE INCLINAÇÃO
// Ao inclinar o sensor, a gravidade se divide entre os eixos X, Y e Z. 
// A fomula pictch e roll descobre o ângulo exato de inclinação baseado nessa divisão ângulo em graus.
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

// Quando o foquete estiver em queda livre, sua a aceleração vertival é menor que 1,96 m/s².
// Essa condição da aceleração vertical deve durar por mais de 100 milissegundos, para o codigo assumir a queda livre.
bool isFalling(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float aceleracaoVertical = abs(a.acceleration.x);

  if(aceleracaoVertical > 1.96){
    lastTime = millis();
    falling = false;
  }else{
    if(millis() - lastTime > 100){
      falling = true;
    }
  }

  return falling;
}