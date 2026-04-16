#include <SPI.h>

// ===== PINOS (ajuste se necessário) =====
#define FLASH_CS 33
#define FLASH_SCK 18
#define FLASH_MISO 19
#define FLASH_MOSI 23

// ===== COMANDOS =====
#define CMD_JEDEC_ID 0x9F
#define CMD_WRITE_ENABLE 0x06
#define CMD_PAGE_PROGRAM 0x02
#define CMD_READ_DATA 0x03

void writeEnable() {
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(FLASH_CS, HIGH);
}

void readJEDEC() {
  Serial.println("\n--- JEDEC ID ---");

  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_JEDEC_ID);

  byte m = SPI.transfer(0x00);
  byte t = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);

  digitalWrite(FLASH_CS, HIGH);

  Serial.print("ID: 0x");
  Serial.print(m, HEX); Serial.print(" ");
  Serial.print(t, HEX); Serial.print(" ");
  Serial.println(c, HEX);
}

void writeData(uint32_t addr, byte data) {
  writeEnable();

  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_PAGE_PROGRAM);

  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);

  SPI.transfer(data);
  digitalWrite(FLASH_CS, HIGH);

  delay(10);
}

byte readData(uint32_t addr) {
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_READ_DATA);

  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);

  byte data = SPI.transfer(0x00);

  digitalWrite(FLASH_CS, HIGH);
  return data;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== TESTE FLASH ===");

  // Inicializa SPI
  SPI.begin(FLASH_SCK, FLASH_MISO, FLASH_MOSI, FLASH_CS);

  pinMode(FLASH_CS, OUTPUT);
  digitalWrite(FLASH_CS, HIGH);

  // ===== TESTE 1: IDENTIFICAÇÃO =====
  readJEDEC();

  // ===== TESTE 2: ESCRITA E LEITURA =====
  Serial.println("\n--- TESTE DE ESCRITA ---");

  uint32_t endereco = 0x000000;
  byte valor = 0xAB;

  Serial.print("Escrevendo: 0x");
  Serial.println(valor, HEX);

  writeData(endereco, valor);

  byte leitura = readData(endereco);

  Serial.print("Lido: 0x");
  Serial.println(leitura, HEX);

  if (leitura == valor) {
    Serial.println("✅ FLASH OK");
  } else {
    Serial.println("❌ ERRO NA FLASH");
  }
}

void loop() {}