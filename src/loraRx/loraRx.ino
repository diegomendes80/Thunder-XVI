#include <SPI.h>
#include <LoRa.h>
#include "BluetoothSerial.h" // Biblioteca nativa do ESP32

// // Verificação de configuração do Bluetooth
// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// BluetoothSerial SerialBT;

// Definição dos pinos do RECEPTOR
const int csPin = 5;          
const int resetPin = 22;      
const int irqPin = 32;    
int count = 0;     // Contador de pacotes recebidos

void setup() {

  Serial.begin(115200);
  delay(200);
  Serial.println("\n[Boot] ESP32 acordou com sucesso!");

  // 3. Inicialização padrão do SPI e LoRa
  SPI.begin(18, 19, 13, 5); 
  LoRa.setPins(csPin, resetPin, irqPin);

  Serial.println("[LoRa] Tentando iniciar...");
  if (!LoRa.begin(433E6)) {
    Serial.println("[Erro] Falha ao iniciar o LoRa!");
    // Pequeno truque se o rádio estiver "atordoado" pelo boot frio:
    delay(300);
    if (!LoRa.begin(433E6)) {
      Serial.println("[Erro Definitivo] Hardware LoRa não responde.");
      while (1);
    }
  }

  Serial.println("[LoRa] Iniciado com sucesso!");
  delay(200);


  Serial.println("[Sistema] Pronto. Aguardando pacotes...");
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    count++; 
    String rawData = "";
    
    while (LoRa.available()) {
      rawData += (char)LoRa.read();
    }

    // Separa os dados recebidos pelas vírgulas
    String alt  = getValue(rawData, ',', 0);
    String pres = getValue(rawData, ',', 1);
    String temp = getValue(rawData, ',', 2);
    String ax   = getValue(rawData, ',', 3);
    String ay   = getValue(rawData, ',', 4);
    String az   = getValue(rawData, ',', 5);
    String ogx  = getValue(rawData, ',', 6);
    String ogy  = getValue(rawData, ',', 7);
    String ogz  = getValue(rawData, ',', 8);
    String lat  = getValue(rawData, ',', 9);
    String lon  = getValue(rawData, ',', 10);

    // Remonta o painel visual direto no receptor!
    String painel = "====================================\n";
    painel += "          THUNDER-XVI - TELEMETRIA  \n";
    painel += "====================================\n";
    painel += "  [AMBIENTE]\n";
    painel += "  Altitude:    " + alt + " m\n";
    painel += "  Pressao:     " + pres + " Pa\n";
    painel += "  Temperatura: " + temp + " C\n";
    painel += "------------------------------------\n";
    painel += "  [ACELEROMETRO] (m/s2)\n";
    painel += "  AX: " + ax + " | AY: " + ay + " | AZ: " + az + "\n";
    painel += "------------------------------------\n";
    painel += "  [GIROSCOPIO] (rad/s)\n";
    painel += "  GX: " + ogx + " | GY: " + ogy + " | GZ: " + ogz + "\n";
    painel += "------------------------------------\n";
    painel += "  [GPS]\n";
    painel += "  Latitude:  " + lat + "\n";
    painel += "  Longitude: " + lon + "\n";
    painel += "====================================\n";
    painel += "RSSI: " + String(LoRa.packetRssi()) + " | Pacote: " + String(count) + "\n\n";

 
    Serial.print(painel);
    // SerialBT.print(painel);
  }
}