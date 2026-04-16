#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

//SDA => D21
//SCL =< D22

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);

 
  Wire.begin();

 
  if (!mpu.begin()) {
    Serial.println("Erro ao encontrar o MPU6050!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 iniciado com sucesso!");


  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Acelerômetro (m/s^2): ");
  Serial.print("X: "); Serial.print(a.acceleration.x);
  Serial.print(" Y: "); Serial.print(a.acceleration.y);
  Serial.print(" Z: "); Serial.println(a.acceleration.z);

  Serial.print("Giroscópio (rad/s): ");
  Serial.print("X: "); Serial.print(g.gyro.x);
  Serial.print(" Y: "); Serial.print(g.gyro.y);
  Serial.print(" Z: "); Serial.println(g.gyro.z);

  Serial.print("Temperatura: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  Serial.println("-----------------------------");
  delay(1000); 
}
