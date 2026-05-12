#include <TinyGPSPlus.h>
#define RX_GPS 16
#define TX_GPS 17



TinyGPSPlus gps;
HardwareSerial GPS_Serial(2); // UART2 D16 e D17

void initGPS(){
  // Serial.begin(115200); // PC: 115200 para ser rápido
  GPS_Serial.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);

  Serial.println("GPS Iniciado");

}

void getPosition(double v[2]){
  if(gps.location.isValid()){
    v[0] = gps.location.lat();
    v[1] = gps.location.lng();
  }else{
    v[0] = 0.0;
    v[1] = 0.0;

  }
}