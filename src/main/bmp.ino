#include <Wire.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bmp;

//#define ALTITUDE_LOCAL 21 // Fortaleza - Se ficar melhor uso a altitude local


void initBMP() {
  Serial.println("Iniciando BMP280...");

  //Tenta inciar o BMP280 testando os dois endereços padrões para o modelo utilizado
  //caso não seja encontrado entra num loop infinito de erro
  if(!bmp.begin(0x76) && !bmp.begin(0x77)){
    Serial.println("Módulo BMP280 Não Encontrado");
    while(1); delay(10);
  }

  Serial.println("BMP280 Inciado");
}

//funções para aquisição dos três dados mais importantes. Serão usadas no código main
float getTemperature() { return bmp.readTemperature(); }
float getPressure()    { return bmp.readPressure(); }
float getAltitude()    { return bmp.readAltitude(1016.0); }
