#include <TinyGPSPlus.h>
#include "BluetoothSerial.h" 


//CONFIGURAÇÕES DO BLUETOOTH:
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

  TinyGPSPlus gps;

  // UART1
  HardwareSerial GPS_Serial(1);

#define RX_GPS 16
#define TX_GPS 17

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("=== TESTE GPS NEO-6M ===");

  // Inicializa GPS
  GPS_Serial.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);

  Serial.println("GPS iniciado...");
}

void loop() {

  // Lê dados do GPS
  while (GPS_Serial.available()) {
    gps.encode(GPS_Serial.read());
  }

  // Se tiver nova localização
  
    Serial.println("\n--- LOCALIZACAO ---");

    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);

    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);

    Serial.print("Satelites: ");
    Serial.println(gps.satellites.value());

    Serial.print("Precisao HDOP: ");
    Serial.println(gps.hdop.hdop());

    Serial.print("Altitude (m): ");
    Serial.println(gps.altitude.meters());
 

  delay(1000);
}