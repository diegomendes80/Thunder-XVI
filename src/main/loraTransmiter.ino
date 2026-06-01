#include <LoRa.h>


int count=0;

void initLoRa(){
 
  Serial.println("Iniciando SPI...");

  Serial.println("LoRa Sender - Iniciando...");
  LoRa.setPins(csPin, resetPin, irqPin);

  if(!LoRa.begin(433E6)){
    SPI.end();
    SPI.begin(SCK, MOSI, MISO, csPin);
    if(!LoRa.begin(433E6)){
      Serial.println("Falha ao inician módulo LoRa");
      while(1);
    }
  }

  LoRa.setSpreadingFactor(9); //testando primeiro com 9. com 7 (padrão) conseguimos ~280 m com -108 RSSI

  Serial.println("LoRa Iniciado");
}

void sendData(String data){
  Serial.println("Enviando pacote");

  //inicia o pacote
  LoRa.beginPacket();
  // LoRa.print(text);
  LoRa.println(data);
  LoRa.endPacket();

}