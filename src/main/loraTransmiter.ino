#include <LoRa.h>

// Definição dos pinos para o TRANSMISSOR
#define csPin 5         // NSS pino do chip
#define resetPin 22     // RST
#define irqPin 26        // DIO0 pino de interrupção
#define SCK 18
#define MISO 19
#define MOSI 23



int count=0;

void initLoRa(){
 
  Serial.println("Iniciando SPI...");

  Serial.println("LoRa Sender - Iniciando...");
  LoRa.setPins(csPin, resetPin, irqPin);

  if(!LoRa.begin(433E6)){
    Serial.println("Falha ao inician módulo LoRa");
    while(1);
  }

  LoRa.setSpreadingFactor(9); //testando primeiro com 9. com 7 (padrão) conseguimos ~280 m com -108 RSSI

  Serial.println("LoRa Iniciado");
}

void sendData(float data, String text){
  Serial.println("Enviando pacote");

  //inicia o pacote
  LoRa.beginPacket();
  LoRa.print(text);
  LoRa.println(data);
  LoRa.endPacket();

}
