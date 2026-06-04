#include <SPI.h>
#include <LoRa.h>
#include "BluetoothSerial.h" // Biblioteca nativa do ESP32

// Verificação de configuração do Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// Definição dos pinos do RECEPTOR
const int csPin = 5;          
const int resetPin = 22;      
const int irqPin = 4;    
int count = 0;     // Contador de pacotes recebidos

void setup() {
  Serial.begin(115200);
  
 
  SerialBT.begin("Receptor_LoRa_Diego"); 
  Serial.println("Bluetooth iniciado! Já pode parear no celular.");

  SPI.begin(18, 19, 25, 5); 
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao iniciar o LoRa!");
    SerialBT.println("Erro: Falha no LoRa!");
    while (1);
  }

  LoRa.setSpreadingFactor(9); // Alinhado com o Transmissor (SF9)

  Serial.println("Aguardando pacotes...");
  SerialBT.println("Conectado! Aguardando dados LoRa...");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    
    count++; 
    
    String data = "";
    
    // Lê o conteúdo do pacote LoRa
    while (LoRa.available()) {
      data += (char)LoRa.read();
    }

    // --- Envia para o Serial Monitor (Cabo USB) ---
    Serial.println(data);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Pacote: " + String(count)); 
    Serial.println("-----------------------");

    // --- Envia para o Celular (Bluetooth) ---
    SerialBT.println(data);
    SerialBT.println("RSSI: " + String(LoRa.packetRssi()));
    SerialBT.println("Pacote: " + String(count)); 
    SerialBT.println("-----------------------");
  }
}