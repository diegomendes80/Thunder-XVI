# 🚀 Thunder XVI — Sistema Eletrônico de Telemetria e Recuperação

<div align="center">

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-orange)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Communication](https://img.shields.io/badge/communication-LoRa-green)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

Sistema eletrônico embarcado desenvolvido para o foguete **Thunder XVI**, do grupo de desenvolvimento aeroespacial **GDAE**.

Responsável por telemetria, aquisição de dados, localização e acionamento do sistema de recuperação.

</div>

---

# 📌 Visão Geral

O sistema eletrônico do **Thunder XVI** foi projetado para operar de forma autônoma durante todas as fases do voo, realizando:

- Monitoramento em tempo real dos parâmetros de voo
- Envio de telemetria via rádio LoRa
- Registro de dados em memória flash
- Determinação do apogeu
- Acionamento do sistema de recuperação (paraquedas)
- Rastreamento geográfico via GPS

A arquitetura é baseada no microcontrolador **ESP32**, integrando múltiplos sensores e módulos de comunicação.

---

# 🧩 Arquitetura do Sistema

```text
                +-------------------+
                |       ESP32       |
                | Núcleo Principal  |
                +---------+---------+
                          |
        -----------------------------------------
        |         |         |        |          |
      BMP       MPU       GPS      LoRa      Flash
   Altitude   IMU 6DOF  Localização Rádio   Memória
```

---

# ⚙️ Objetivos do Sistema

## ✅ Recuperação do Foguete
Garantir o acionamento do paraquedas no momento adequado através da análise de:

- Altitude
- Pressão atmosférica
- Inclinação e queda do foguete

---

## 📡 Telemetria em Tempo Real
Transmitir informações do voo utilizando comunicação LoRa de longa distância.

---

## 💾 Registro de Dados
Armazenar dados do voo para análise posterior e validação experimental.

---

# 🔧 Módulos Utilizados

| Módulo | Função |
|---|---|
| BMP280/BMP180 | Temperatura, pressão e cálculo de altitude |
| MPU6050 | Aceleração e orientação nos eixos X, Y e Z |
| GPS | Coordenadas geográficas e localização |
| LoRa SX1278 | Comunicação sem fio de longa distância |
| Memória Flash | Armazenamento persistente dos dados |
| ESP32 | Processamento principal e controle do sistema |

---

# 📊 Funções de Cada Subsistema

## 🌡️ BMP — Sensor Barométrico
Responsável pela leitura de:

- Pressão atmosférica
- Temperatura
- Altitude relativa

Utilizado principalmente para:

- Detecção de apogeu
- Perfil altimétrico do voo

---

## 📈 MPU — Unidade de Medição Inercial
Responsável por medir:

- Aceleração linear
- Inclinação
- Movimento angular

Utilizado para:

- Detecção de queda
- Estabilidade do voo
- Segurança no acionamento do paraquedas

---

## 🛰️ GPS — Geolocalização
Responsável pelo fornecimento de:

- Latitude
- Longitude
- Altitude GPS
- Velocidade

Utilizado para recuperação do foguete após o pouso.

---

## 📶 LoRa — Telemetria
Responsável pela transmissão dos dados de voo em tempo real.

Características:

- Longo alcance
- Baixo consumo energético
- Comunicação robusta para aplicações aeroespaciais

---

## 💽 Flash Memory
Responsável pelo armazenamento dos dados de:

- Sensores
- Telemetria
- Eventos críticos do voo

Permitindo análise pós-voo.

---

# 🪂 Lógica de Acionamento do Paraquedas

O acionamento do sistema de recuperação é baseado na combinação de:

- Detecção de apogeu via sensor barométrico
- Identificação de queda através da IMU

## Critérios principais:

```text
Altitude ↓
+
Ângulo de queda detectado
=
Acionamento do paraquedas
```

Essa abordagem reduz falsos acionamentos e aumenta a confiabilidade do sistema.

---

# 📡 Dados Transmitidos

Exemplo de dados enviados via LoRa:

```json
{
  "altitude": 312.5,
  "temperatura": 28.4,
  "pressao": 96421,
  "ax": 0.12,
  "ay": -0.04,
  "az": 9.71,
  "latitude": -3.73,
  "longitude": -38.52
}
```

---

# 🛠️ Tecnologias Utilizadas

- ESP32
- Arduino Framework
- Comunicação LoRa
- Sensores I2C
- GPS UART
- Memória Flash SPI
- C/C++

---

# 📂 Estrutura do Projeto

```text
/
├── firmware/
│   ├── sensores/
│   ├── telemetria/
│   ├── armazenamento/
│   ├── recuperacao/
│   └── main.ino
│
├── docs/
├── esquematicos/
├── testes/
└── README.md
```

---

# 🎯 Responsabilidades do Sistema

| Função | Responsável |
|---|---|
| Acionamento do paraquedas | BMP + MPU |
| Localização | GPS |
| Telemetria | LoRa |
| Armazenamento de dados | Flash |
| Processamento central | ESP32 |

---

# 🔬 Aplicações

- Foguetes experimentais
- Telemetria aeroespacial
- Estudos de voo
- Aquisição embarcada de dados
- Sistemas autônomos de recuperação

---

# 👨‍🚀 Equipe

Projeto desenvolvido pelo grupo:

## **GDAE — Grupo de Desenvolvimento Aeroespacial**

---

# 📄 Licença

Este projeto é destinado para fins acadêmicos, experimentais e educacionais.

Licença sugerida: MIT License.
