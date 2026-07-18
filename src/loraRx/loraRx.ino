#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Thunder_XVI_Painel";
const char* password = "thunderrocket"; 

WebServer server(80);

const int csPin = 5;          
const int resetPin = 22;      
const int irqPin = 32;    
int count = 0;     

String alt = "0.0", pres = "0.0", temp = "0.0";
String ax = "0.0", ay = "0.0", az = "0.0";
String ogx = "0.0", ogy = "0.0", ogz = "0.0";
String lat = "0.000000", lon = "0.000000";
String lora_rssi = "0";

// --- INTERFACE RESPONSIVA V3.2 (RESPONSIVA + TARA DA ALTITUDE) ---
const char HTML_DASHBOARD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta charset="utf-8">
    <title>THUNDER-XVI Telemetry Core v3.2</title>
    <style>
        :root {
            --bg-main: #0b0f19;
            --bg-card: #131a2b;
            --border-color: #1e2942;
            --text-muted: #64748b;
            --text-light: #f8fafc;
            --neon-blue: #38bdf8;
            --neon-green: #10b981;
            --neon-amber: #f59e0b;
        }
        
        body { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background-color: var(--bg-main); color: var(--text-light); margin: 0; padding: 20px; box-sizing: border-box; }
        *, *:before, *:after { box-sizing: inherit; }

        /* Barra Superior Responsiva */
        .top-bar { display: flex; justify-content: space-between; align-items: center; padding: 15px 25px; background-color: var(--bg-card); border-radius: 10px; border: 1px solid var(--border-color); margin-bottom: 20px; gap: 15px; }
        .top-bar h1 { margin: 0; font-size: 20px; font-weight: 700; letter-spacing: 1.5px; color: var(--text-light); }
        .top-bar h1 span { color: var(--neon-blue); }
        
        .status-container { display: flex; align-items: center; gap: 15px; font-size: 13px; font-weight: 600; color: var(--text-muted); }
        .status-item { display: flex; align-items: center; gap: 8px; white-space: nowrap; }
        .pulse-dot { width: 8px; height: 8px; background-color: var(--neon-green); border-radius: 50%; animation: pulse 1.5s infinite; }
        @keyframes pulse { 0% { opacity: 0.3; } 50% { opacity: 1; } 100% { opacity: 0.3; } }

        /* Grid Principal Dinâmico */
        .main-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
        
        /* Cartões e Alinhamentos */
        .panel-card { background-color: var(--bg-card); border: 1px solid var(--border-color); border-radius: 10px; padding: 20px; display: flex; flex-direction: column; justify-content: space-between; }
        
        .card-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; gap: 10px; }
        .card-header h2 { margin: 0; font-size: 13px; text-transform: uppercase; letter-spacing: 1px; color: var(--text-muted); border-left: 3px solid var(--neon-blue); padding-left: 10px; }
        
        /* Botão Técnico de Tara */
        .tare-button { background-color: transparent; border: 1px solid var(--border-color); color: var(--text-light); padding: 5px 12px; border-radius: 4px; font-size: 11px; font-weight: 700; cursor: pointer; transition: all 0.2s ease; letter-spacing: 0.5px; text-transform: uppercase; }
        .tare-button:hover { border-color: var(--neon-amber); color: var(--neon-amber); background: rgba(245, 158, 11, 0.03); }
        .tare-button:active { transform: scale(0.96); }

        /* Display Massivo de Altitude */
        .primary-display { display: flex; flex-direction: column; align-items: center; justify-content: center; padding: 30px 10px; background: rgba(56, 189, 248, 0.02); border-radius: 8px; border: 1px dashed rgba(56, 189, 248, 0.15); margin-bottom: 5px; }
        .primary-value { font-size: 54px; font-weight: 800; color: var(--neon-blue); font-variant-numeric: tabular-nums; line-height: 1; text-shadow: 0 0 20px rgba(56, 189, 248, 0.1); }
        .primary-unit { font-size: 12px; color: var(--text-muted); text-transform: uppercase; margin-top: 8px; letter-spacing: 1px; }

        /* Linhas de Dados */
        .metric-row { display: flex; justify-content: space-between; align-items: center; padding: 12px 0; border-bottom: 1px solid rgba(30, 41, 66, 0.4); }
        .metric-row:last-child { border-bottom: none; }
        .metric-label { font-size: 14px; color: var(--text-muted); }
        .metric-value { font-size: 17px; font-weight: 600; font-variant-numeric: tabular-nums; }
        
        /* Grades Internas de Eixos (X, Y, Z) */
        .vector-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-top: 5px; }
        .vector-box { background: rgba(11, 15, 25, 0.4); padding: 12px 8px; border-radius: 6px; text-align: center; border: 1px solid rgba(30, 41, 66, 0.6); }
        .vector-axis { font-size: 10px; color: var(--text-muted); display: block; margin-bottom: 4px; font-weight: bold; }
        .vector-num { font-size: 15px; font-weight: 600; font-variant-numeric: tabular-nums; }

        /* Subgrid de GPS */
        .gps-subgrid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }

        .footer { display: flex; justify-content: space-between; align-items: center; margin-top: 25px; padding: 15px 5px; border-top: 1px solid var(--border-color); font-size: 12px; color: var(--text-muted); gap: 15px; }

        /* --- MEDIA QUERIES PARA RESPONSIVIDADE EM CELULARES --- */
        @media (max-width: 768px) {
            body { padding: 12px; }
            .top-bar { flex-direction: column; text-align: center; padding: 15px; gap: 12px; }
            .status-container { justify-content: center; width: 100%; }
            
            .main-grid { grid-template-columns: 1fr; gap: 15px; }
            .panel-card { grid-column: span 1 !important; grid-row: span 1 !important; padding: 15px; }
            
            .primary-value { font-size: 46px; }
            .gps-subgrid { grid-template-columns: 1fr; gap: 0; }
            .footer { flex-direction: column; text-align: center; gap: 10px; }
        }
    </style>
</head>
<body>

    <!-- Barra de Controle Superior -->
    <div class="top-bar">
        <h1>THUNDER-XVI <span>// TELEMETRY CORE</span></h1>
        <div class="status-container">
            <div class="status-item">
                <div class="pulse-dot"></div>
                <span>RX STREAM ACTIVE</span>
            </div>
        </div>
    </div>

    <!-- Layout Modular Grid -->
    <div class="main-grid">
        
        <!-- CARD 1: PERFIL DE ALTITUDE (COM TARA) -->
        <div class="panel-card" style="grid-row: span 2;">
            <div class="card-header">
                <h2>Métrica de Voo Primária</h2>
                <button class="tare-button" id="btn-tare-alt">Zerar Alt.</button>
            </div>
            <div class="primary-display">
                <div class="primary-value" id="alt">0.00</div>
                <div class="primary-unit">Altitude Relativa (Metros)</div>
            </div>
            <div>
                <div class="metric-row">
                    <span class="metric-label">Pressão Atmosférica</span>
                    <span class="metric-value"><span id="pres">0.00</span> <small style="color:var(--text-muted); font-size:11px;">Pa</small></span>
                </div>
                <div class="metric-row">
                    <span class="metric-label">Temperatura do Core</span>
                    <span class="metric-value"><span id="temp">0.0</span> <small style="color:var(--text-muted); font-size:11px;">°C</small></span>
                </div>
            </div>
        </div>

        <!-- CARD 2: DINÂMICA INERCIAL (ACELERÔMETRO) -->
        <div class="panel-card">
            <div class="card-header">
                <h2>Aceleração Linear (m/s²)</h2>
            </div>
            <div class="vector-grid">
                <div class="vector-box"><span class="vector-axis">EIXO X</span><span class="vector-num" id="ax">0.00</span></div>
                <div class="vector-box"><span class="vector-axis">EIXO Y</span><span class="vector-num" id="ay">0.00</span></div>
                <div class="vector-box"><span class="vector-axis" style="color:var(--neon-blue);">EIXO Z</span><span class="vector-num" style="color:var(--neon-blue);" id="az">0.00</span></div>
            </div>
        </div>

        <!-- CARD 3: VELOCIDADE ANGULAR (GIROSCÓPIO) -->
        <div class="panel-card">
            <div class="card-header">
                <h2>Giroscópio Dinâmico (rad/s)</h2>
            </div>
            <div class="vector-grid">
                <div class="vector-box"><span class="vector-axis">VEL. GX</span><span class="vector-num" id="gx">0.00</span></div>
                <div class="vector-box"><span class="vector-axis">VEL. GY</span><span class="vector-num" id="gy">0.00</span></div>
                <div class="vector-box"><span class="vector-axis">VEL. GZ</span><span class="vector-num" id="gz">0.00</span></div>
            </div>
        </div>

        <!-- CARD 4: COORDENADAS GEOFÍSICAS (GPS) -->
        <div class="panel-card" style="grid-column: span 2;">
            <div class="card-header">
                <h2>Mapeamento de Coordenadas GPS</h2>
            </div>
            <div class="gps-subgrid">
                <div class="metric-row">
                    <span class="metric-label">Latitude Global</span>
                    <span class="metric-value" style="color: var(--neon-green);" id="lat">0.000000</span>
                </div>
                <div class="metric-row">
                    <span class="metric-row-mobile metric-row"></span>
                    <span class="metric-label">Longitude Global</span>
                    <span class="metric-value" style="color: var(--neon-green);" id="lon">0.000000</span>
                </div>
            </div>
        </div>

    </div>

    <!-- Rodapé Técnico -->
    <div class="footer">
        <div>NÍVEL DE SINAL (RSSI): <span id="rssi" style="color: var(--neon-blue); font-weight: bold;">0</span> dBm</div>
        <div>PACOTES PROCESSADOS: <span id="count" style="color: var(--neon-green); font-weight: bold;">0</span></div>
    </div>

    <script>
        let tareAltitude = 0;
        let currentRawAltitude = 0;

        // Evento para capturar e aplicar a tara de altitude no cliente browser
        document.getElementById('btn-tare-alt').addEventListener('click', function() {
            tareAltitude = currentRawAltitude;
        });

        setInterval(function() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Armazena valor bruto para possibilitar calculo da tara continuo
                    currentRawAltitude = parseFloat(data.alt) || 0.0;
                    let displayAltitude = currentRawAltitude - tareAltitude;

                    // Atualizacoes textuais estruturadas
                    document.getElementById('alt').innerText = displayAltitude.toFixed(2);
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
                .catch(err => console.log("Erro de comunicacao interna: ", err));
        }, 250);
    </script>
</body>
</html>
)rawliteral";

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
  String res = found > index ? data.substring(strIndex[0], strIndex[1]) : "";
  res.trim();
  return res;
}

void handleRoot() {
  server.send(200, "text/html", HTML_DASHBOARD);
}

void handleData() {
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
  Serial.println("\n[Boot] ESP32 Core v3.2.0 Inicializado!");

  SPI.begin(18, 19, 13, 5); 
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(433E6)) {
    Serial.println("[Erro] Falha no modulo LoRa!");
    while (1);
  }

  WiFi.softAP(ssid, password);
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("[Online] Painel Responsivo pronto para dispositivos moveis.");
}

void loop() {
  server.handleClient();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    count++; 
    String rawData = "";
    while (LoRa.available()) {
      rawData += (char)LoRa.read();
    }
    rawData.trim(); 

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
  }
}