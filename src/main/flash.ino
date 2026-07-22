#include <Arduino.h>
#include <SPI.h>

// --- PINAGEM DA FLASH SPI ---
#define FLASH_CS   33
#define FLASH_SCK  18
#define FLASH_MISO 19
#define FLASH_MOSI 23

// --- COMANDOS SPI FLASH (Ex: Winbond W25Qxx) ---
#define CMD_JEDEC_ID     0x9F
#define CMD_WRITE_ENABLE 0x06
#define CMD_PAGE_PROGRAM 0x02
#define CMD_READ_DATA    0x03
#define CMD_SECTOR_ERASE 0x20  // Apaga setor de 4KB
#define CMD_CHIP_ERASE   0xC7  // Comando Chip Erase da W25Qxx
#define CMD_READ_STATUS_REG1 0x05

// --- VARIÁVEIS GLOBAIS DE CONTROLE ---
uint32_t currentFlashAddress = 0x000000;
const uint32_t FLASH_MAX_SIZE = 0x400000; 



void writeEnable() {
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(FLASH_CS, HIGH);
}

void readJEDEC() {
  Serial.println("\n--- CHECAGEM DA FLASH (JEDEC ID) ---");
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_JEDEC_ID);

  byte m = SPI.transfer(0x00);
  byte t = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);

  digitalWrite(FLASH_CS, HIGH);

  Serial.print("ID do fabricante/dispositivo: 0x");
  Serial.print(m, HEX); Serial.print(" ");
  Serial.print(t, HEX); Serial.print(" ");
  Serial.println(c, HEX);
}

void eraseSector(uint32_t addr) {
  writeEnable();

  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_SECTOR_ERASE);
  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);
  digitalWrite(FLASH_CS, HIGH);

  delay(50); // Aguarda o ciclo de apagamento do setor (tSE)
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

  delay(1); // Delay rápido para gravação por byte
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

// =========================================================================
// MÉTODOS DE ALTO NÍVEL (TELEMETRIA, LIMPEZA E RECUPERAÇÃO)
// =========================================================================

// Procura o primeiro byte virgem (0xFF) para continuar a escrita sem sobrescrever dados antigos
uint32_t findNextFreeAddress() {
  Serial.println("[Flash] Mapeando espaço disponível...");
  uint32_t addr = 0x000000;
  
  while (addr < FLASH_MAX_SIZE) {
    if (readData(addr) == 0xFF) {
      return addr; // Encontrou o início do espaço limpo
    }
    addr++;
  }
  return 0x000000; // Caso a memória esteja 100% cheia
}

// Escreve o pacote de string completo na Flash byte a byte
void logTelemetryToFlash(String telemetryPacket) {
  telemetryPacket += "\n"; // Quebra de linha para separar os pacotes

  for (size_t i = 0; i < telemetryPacket.length(); i++) {
    if (currentFlashAddress >= FLASH_MAX_SIZE) {
      Serial.println("[Flash ERRO] Memória totalmente cheia!");
      return;
    }
    
    byte b = (byte)telemetryPacket.charAt(i);
    writeData(currentFlashAddress, b);
    currentFlashAddress++;
  }
}

// Imprime todo o conteúdo gravado da Flash no Monitor Serial
void dumpFlashTelemetry() {
  Serial.println("\n==========================================");
  Serial.println("      DUMP DE TELEMETRIA DA FLASH         ");
  Serial.println("==========================================");

  if (currentFlashAddress == 0) {
    Serial.println("Nenhum dado gravado na memória.");
  } else {
    for (uint32_t addr = 0; addr < currentFlashAddress; addr++) {
      char character = (char)readData(addr);
      Serial.print(character);
    }
  }

  Serial.println("==========================================");
  Serial.println("            FIM DO DUMP                   \n");
}

// Função manual para formatar os setores da memória na bancada
void eraseAllTelemetry(uint32_t numSectorsToErase = 10) {
  Serial.println("\n[Flash] Apagando setores para novo voo...");
  
  for (uint32_t i = 0; i < numSectorsToErase; i++) {
    uint32_t sectorAddress = i * 0x1000; // Cada setor tem 4096 bytes (4KB)
    eraseSector(sectorAddress);
  }
  
  currentFlashAddress = 0x000000;
  Serial.println("[Flash] Memória limpa e pronta para uso!");
}

// Inicialização segura que preserva o histórico em caso de reset
void initFlash() {
  SPI.begin(FLASH_SCK, FLASH_MISO, FLASH_MOSI, FLASH_CS);
  pinMode(FLASH_CS, OUTPUT);
  digitalWrite(FLASH_CS, HIGH);

  readJEDEC();

  // Encontra de onde deve continuar gravando
  currentFlashAddress = findNextFreeAddress();
  
  Serial.print("[Flash] Pronta! Escrevendo a partir do endereço: 0x");
  Serial.println(currentFlashAddress, HEX);
}

bool isFlashBusy() {
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_READ_STATUS_REG1);
  byte status = SPI.transfer(0x00);
  digitalWrite(FLASH_CS, HIGH);
  
  // O Bit 0 (BUSY) fica em '1' enquanto a Flash está gravando ou apagando
  return (status & 0x01); 
}

// =========================================================================
// FUNÇÃO PRINCIPAL: APAGA A MEMÓRIA INTEIRA (FULL CHIP ERASE)
// =========================================================================
void eraseChipFull() {
  Serial.println("\n==================================================");
  Serial.println("  [FLASH] ATENÇÃO: INICIANDO APAGAMENTO COMPLETO! ");
  Serial.println("  NÃO desligue a alimentação do ESP32 durante o processo...");
  Serial.println("==================================================");

  // 1. Habilita escrita
  writeEnable();

  // 2. Envia o comando de Chip Erase
  digitalWrite(FLASH_CS, LOW);
  SPI.transfer(CMD_CHIP_ERASE);
  digitalWrite(FLASH_CS, HIGH);

  unsigned long startT = millis();

  // 3. Aguarda o término acompanhando o registrador BUSY
  Serial.print("[FLASH] Apagando 32Mbit (4MB)");
  while (isFlashBusy()) {
    Serial.print(".");
    delay(500); // Imprime um ponto a cada meio segundo
  }

  // 4. Reseta o ponteiro de escrita para o início da memória (0x000000)
  currentFlashAddress = 0x000000;

  float tempoTotal = (millis() - startT) / 1000.0;
  Serial.println();
  Serial.print("[FLASH] Concluído em ");
  Serial.print(tempoTotal, 1);
  Serial.println(" segundos!");
  Serial.println("[FLASH] Memória 100% virgem (0xFF) e pronta para novos voos.");
  Serial.println("==================================================\n");
}


