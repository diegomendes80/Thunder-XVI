#include <ESP32Servo.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVO_PIN 4

Servo myServo;

// UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-ab12-ab12-ab12-abcd12345678"

BLECharacteristic *pCharacteristic;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    
    String value = pCharacteristic->getValue().c_str();

    if (value.length() > 0) {
      int angle = value.toInt();

      if (angle >= 0 && angle <= 180) {
        Serial.print("Ângulo recebido: ");
        Serial.println(angle);

        myServo.write(angle);
      } else {
        Serial.println("Valor fora da faixa (0-180)");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Servo
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(90);

  // BLE
  BLEDevice::init("ESP32_Servo");

  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("90");

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE iniciado!");
  Serial.println("Nome: ESP32_Servo");
}

void loop() {
}