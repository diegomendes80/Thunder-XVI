#include <TinyGPSPlus.h>

TinyGPSPlus gps;

// UART1
HardwareSerial GPS_Serial(1);

#define RX_GPS 16
#define TX_GPS 17

void setup() {
  Serial.begin(115200);
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
  if (gps.location.isUpdated()) {
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
  } 
  else {
    Serial.println("Aguardando sinal GPS...");
  }

  delay(1000);
}