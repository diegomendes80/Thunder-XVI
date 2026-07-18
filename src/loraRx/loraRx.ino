#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WebServer.h>

// Configurações da rede Wi-Fi que o ESP32 vai criar
const char* ssid = "Thunder_XVI_Painel";
const char* password = "thunderrocket"; // Mínimo 8 caracteres

WebServer server(80);

// Definição dos pinos do RECEPTOR (Atualizados e soldados)
const int csPin = 5;          
const int resetPin = 22;      
const int irqPin = 32;    
int count = 0;     

// Variáveis Globais para o Servidor Web acessar os dados atualizados
String alt = "0.0", pres = "0.0", temp = "0.0";
String ax = "0.0", ay = "0.0", az = "0.0";
String ogx = "0.0", ogy = "0.0", ogz = "0.0";
String lat = "0.000000", lon = "0.000000";
String lora_rssi = "0";

// --- CÓDIGO INTERNO DA PÁGINA WEB (HTML + CSS + JAVASCRIPT) ---
const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta charset="utf-8">
    <title>THUNDER-XVI Telemetria</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #0f172a; color: #f8fafc; margin: 0; padding: 15px; }
        .header { text-align: center; padding: 10px; background: linear-gradient(135deg, #1e293b, #0f172a); border-radius: 8px; border: 1px solid #334155; margin-bottom: 15px; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.5); }
        .header h1 { margin: 0; color: #38bdf8; font-size: 24px; letter-spacing: 1px; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 15px; }
        .card { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; padding: 15px; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.3); }
        .card h2 { margin-top: 0; font-size: 16px; color: #94a3b8; border-bottom: 1px solid #334155; padding-bottom: 5px; text-transform: uppercase; }
        .data-row { display: flex; justify-content: space-between; margin: 10px 0; font-size: 18px; }
        .label { color: #cbd5e1; }
        .value { font-weight: bold; color: #f1f5f9; }
        .highlight { color: #4ade80; }
        .highlight-blue { color: #38bdf8; }
        .footer { display: flex; justify-content: space-between; margin-top: 15px; background-color: #111827; padding: 10px; border-radius: 6px; font-size: 14px; color: #64748b; }
    </style>
</head>
<body>

    <div class="header">
        <h1>THUNDER-XVI — DASHBOARD</h1>
    </div>

    <div class="grid">
        <!-- AMBIENTE -->
        <div class="card">
            <h2>[Ambiente]</h2>
            <div class="data-row"><span class="label">Altitude:</span><span class="value highlight" id="alt">0.00</span><span class="label"> m</span></div>
            <div class="data-row"><span class="label">Pressão:</span><span class="value" id="pres">0.00</span><span class="label"> Pa</span></div>
            <div class="data-row"><span class="label">Temperatura:</span><span class="value" id="temp">0.0</span><span class="label"> °C</span></div>
        </div>

        <!-- ACELERÔMETRO -->
        <div class="card">
            <h2>[Acelerômetro] (m/s²)</h2>
            <div class="data-row"><span class="label">Eixo X:</span><span class="value" id="ax">0.00</span></div>
            <div class="data-row"><span class="label">Eixo Y:</span><span class="value" id="ay">0.00</span></div>
            <div class="data-row"><span class="label">Eixo Z:</span><span class="value highlight-blue" id="az">0.00</span></div>
        </div>

        <!-- GIROSCÓPIO -->
        <div class="card">
            <h2>[Giroscópio] (rad/s)</h2>
            <div class="data-row"><span class="label">GX:</span><span class="value" id="gx">0.00</span></div>
            <div class="data-row"><span class="label">GY:</span><span class="value" id="gy">0.00</span></div>
            <div class="data-row"><span class="label">GZ:</span><span class="value" id="gz">0.00</span></div>
        </div>

        <!-- GPS -->
        <div class="card">
            <h2>[GPS]</h2>
            <div class="data-row"><span class="label">Latitude:</span><span class="value" id="lat">0.000000</span></div>
            <div class="data-row"><span class="label">Longitude:</span><span class="value" id="lon">0.000000</span></div>
        </div>
    </div>

    <div class="footer">
        <div>RSSI: <span id="rssi" style="color:#ef4444; font-weight:bold;">0</span> dBm</div>
        <div>Pacotes Recebidos: <span id="count" style="color:#f59e0b; font-weight:bold;">0</span></div>
    </div>

    <script>
        // Requisições assíncronas em background a cada 250ms (Tempo real sem piscar a tela)
        setInterval(function() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('alt').innerText = data.alt;
                    document.getElementById('pres').innerText = data.pres;
                    document.getElementById('temp').innerText = data.temp;
                    document.getElementById('ax').innerText = data.ax;
                    document.getElementById('ay').innerText = data.ay;
                    document.getElementById('az').innerText = data.az;
                    document.getElementById('gx').innerText = data.gx;
                    document.getElementById('gy').innerText = data.gy;
                    document.getElementById('gz').innerText = data.gz;
                    document.getElementById('lat').innerText = data.lat;
                    document.getElementById('lon').innerText = data.lon;
                    document.getElementById('rssi').innerText = data.rssi;
                    document.getElementById('count').innerText = data.count;
                })
                .catch(err => console.log("Erro de conexão: ", err));
        }, 250);
    </script>
</body>
</html>
)rawliteral";

// Função auxiliar para separar a string por vírgulas
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

// Rotas de resposta do Servidor Web
void handleRoot() {
  server.send(200, "text/html", HTML_DASHBOARD);
}

void handleData() {
  // Envia os dados em formato JSON leve para a página
  String json = "{";
  json += "\"alt\":\"" + alt + "\",";
  json += "\"pres\":\"" + pres + "\",";
  json += "\"temp\":\"" + temp + "\",";
  json += "\"ax\":\"" + ax + "\",";
  json += "\"ay\":\"" + ay + "\",";
  json += "\"az\":\"" + az + "\",";
  json += "\"gx\":\"" + ogx + "\",";
  json += "\"gy\":\"" + ogy + "\",";
  json += "\"gz\":\"" + ogz + "\",";
  json += "\"lat\":\"" + lat + "\",";
  json += "\"lon\":\"" + lon + "\",";
  json += "\"rssi\":\"" + lora_rssi + "\",";
  json += "\"count\":" + String(count);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n[Boot] ESP32 acordou com sucesso!");

  // Inicialização do barramento SPI e LoRa
  SPI.begin(18, 19, 13, 5); 
  LoRa.setPins(csPin, resetPin, irqPin);

  Serial.println("[LoRa] Tentando iniciar...");
  if (!LoRa.begin(433E6)) {
    Serial.println("[Erro] Falha ao iniciar o LoRa!");
    delay(300);
    if (!LoRa.begin(433E6)) {
      Serial.println("[Erro Definitivo] Hardware LoRa não responde.");
      while (1);
    }
  }

  Serial.println("[LoRa] Iniciado com sucesso!");
  delay(200);

  // Configuração do Wi-Fi do ESP32 como Ponto de Acesso (Gerador de Rede)
  Serial.println("[Wi-Fi] Criando rede para o Dashboard...");
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("[Wi-Fi] Rede criada: ");
  Serial.println(ssid);
  Serial.print("[Web] Acesse no navegador pelo IP: ");
  Serial.println(IP); // Padrão: 192.168.4.1

  // Configuração das rotas de navegação
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("[Sistema] Pronto. Aguardando pacotes LoRa...");
}

void loop() {
  // Mantém as conexões e requisições do Servidor Web ativas
  server.handleClient();

  // Escuta o rádio LoRa
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    count++; 
    String rawData = "";
    
    while (LoRa.available()) {
      rawData += (char)LoRa.read();
    }

    rawData.trim();

    // Salva nas variáveis globais (alimenta o JSON do Servidor Web)
    alt  = getValue(rawData, ',', 0);
    pres = getValue(rawData, ',', 1);
    temp = getValue(rawData, ',', 2);
    ax   = getValue(rawData, ',', 3);
    ay   = getValue(rawData, ',', 4);
    az   = getValue(rawData, ',', 5);
    ogx  = getValue(rawData, ',', 6);
    ogy  = getValue(rawData, ',', 7);
    ogz  = getValue(rawData, ',', 8);
    lat  = getValue(rawData, ',', 9);
    lon  = getValue(rawData, ',', 10);
    lora_rssi = String(LoRa.packetRssi());

    // Remonta o painel visual direto no monitor serial do PC
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
    painel += "RSSI: " + lora_rssi + " | Pacote: " + String(count) + "\n\n";

    Serial.print(painel);
  }
}