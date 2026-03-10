<div align="center">
  <h1>ESP8266 Smart Relay com Alexa</h1>
  
  <p>
    <strong>Controle um relé (ou módulo Sonoff básico) via Amazon Alexa usando ESP8266</strong><br>
    Integração nativa com Alexa (sem precisar de servidor externo ou Sinric/Ewelink)
  </p>

  <p>
    <img src="https://img.shields.io/badge/ESP8266-✓-green?style=for-the-badge&logo=arduino&logoColor=white" alt="ESP8266">
    <img src="https://img.shields.io/badge/Alexa-✓-orange?style=for-the-badge&logo=amazon-alexa&logoColor=white" alt="Alexa">
    <img src="https://img.shields.io/badge/Status-Em%20desenvolvimento-yellow?style=for-the-badge" alt="Status">
  </p>
</div>

<br>

## ✨ Funcionalidades

- Controle de relé por voz via Amazon Alexa ("Alexa, ligar Smart Relay")
- Reconexão automática à internet
- Comunicação serial com módulos Sonoff/relés que usam protocolo serial (ex: Sonoff Basic R2, Tasmota sem firmware custom, etc.)
- LED de status interno (pode ser adaptado)
- Timeout e watchdog implícito via reinício em falha de Wi-Fi

## Dispositivos compatíveis (testados)

- Sonoff Basic (R2 e versões mais novas)
- Relés com chip HLW8012 + protocolo serial A0 01 00/01 A1/A2
- Qualquer ESP8266 + relé que aceite controle por GPIO + serial

## 📸 Demonstração

<!-- Você pode adicionar GIF ou fotos aqui depois -->
<!-- <img src="https://via.placeholder.com/800x400/222/eee?text=Em+breve:+vídeo+demonstração" alt="Demonstração"> -->

https://user-images.githubusercontent.com/xxxxxx/xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.mp4

## 🚀 Como usar

### 1. Materiais necessários

- Placa NodeMCU / Wemos D1 Mini / ESP-01S / qualquer ESP8266
- Módulo relé compatível (ou Sonoff Basic com solda nos pinos seriais)
- Fonte 5V estável (≥1A recomendado)

### 2. Conexões (para Sonoff Basic modificado)

| ESP8266     | Sonoff/Relé     | Observação                  |
|-------------|------------------|-----------------------------|
| GPIO0       | RX               | Comunicação serial          |
| 3.3V        | VCC              | Alimentação                 |
| GND         | GND              | —                           |
| —           | TX → resistor 1k → GPIO3 (opcional) | Feedback (não implementado) |

### 3. Configuração

1. Abra o arquivo `sketch.ino`
2. Altere as credenciais Wi-Fi:

```cpp
const char* WIFI_SSID     = "SuaRedeWiFi";
const char* WIFI_PASSWORD = "SuaSenhaMuitoBoa";
