#include <TinyGPSPlus.h> // adiciona a biblioteca
#define RX_GPS 16
#define TX_GPS 17
//definiu qual pinos na esp vao ser o tx e o rx


TinyGPSPlus gps;//cria um objeto chamado gps
HardwareSerial GPS_Serial(2); // UART2 D16 e D17//define o protocolo UART

void initGPS(){ //função para iniciar o gps, o void pra dizer que não tem retorno
  // Serial.begin(115200); // PC: 115200 para ser rápido
  GPS_Serial.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);//definiu a velocidade da comuniação e os nomes dos pinos rx e tx
TinyGPSPlus gps;

HardwareSerial GPS_Serial(2); // UART2 D16 e D17

void initGPS(){

  GPS_Serial.begin(9600, SERIAL_8N1, RX_GPS, TX_GPS);

  Serial.println("GPS Iniciado");//exibe a mensagem dizendo que o gps foi iniciado

} 

void getPosition(double v[2]){//cria uma função sem retorno que guarda os valores de posição em uma variável v de 2 elementos
  if(gps.location.isValid()){//cria condição, verificando se os valores são válidos
    v[0] = gps.location.lat();//guarda no elemento 0 de v a posição de latitude
    v[1] = gps.location.lng();//gurda no elemento 1 de v a posição de longitude
  }else{//se os valores de posição nao forem válidos, guarda o valor 0.0 nos espaços de v
void getPosition(double v[2]){
  
  if(gps.location.isValid()){
    v[0] = gps.location.lat();
    v[1] = gps.location.lng();
  }else{
    v[0] = 0.0;
    v[1] = 0.0;

  }
}