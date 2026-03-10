/**
 * Firmware para Controle de Relés via Alexa
 * Versão: 1.0
 * Autor: [Seu Nome]
 * Data: [Data]
 * 
 * Características:
 * - Suporte para ESP32 e ESP8266
 * - Conexão WiFi resiliente
 * - Configuração organizada
 * - Tratamento de erros robusto
 * - Logging serial adequado
 * - Otimizado para uso em produção
 */

// ================= CONFIGURAÇÕES ================= //
// Defina suas credenciais WiFi e nomes dos dispositivos
#define WIFI_SSID       "SUA_REDE_WIFI"
#define WIFI_PASSWORD   "SUA_SENHA"
#define SERIAL_BAUDRATE 115200

// Nomes dos dispositivos na Alexa (máx. 25 caracteres)
const char* DEVICE_NAMES[] = {
  "Luz Quarto",    // Relé 1
  "Luz Sala",      // Relé 2
  "Luz Cozinha",   // Relé 3
  "Ventilador"     // Relé 4
};

// Mapeamento de pinos (ajuste para sua placa)
#ifdef ARDUINO_ARCH_ESP32
  const uint8_t RELAY_PINS[] = {32, 33, 25, 26}; // ESP32
#else
  const uint8_t RELAY_PINS[] = {5, 4, 14, 12};  // ESP8266
#endif

// ================= BIBLIOTECAS ================= //
#ifdef ARDUINO_ARCH_ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// ================= VARIÁVEIS GLOBAIS ================= //
Espalexa espalexa;
bool wifiConnected = false;
const uint8_t NUM_RELAYS = sizeof(RELAY_PINS) / sizeof(RELAY_PINS[0]);

// ================= PROTÓTIPOS ================= //
bool connectWiFi();
void handleRelay(uint8_t relayNum, uint8_t brightness);
void relayCallback(uint8_t brightness, uint8_t relayNum);

// ================= SETUP ================= //
void setup() {
  // Inicialização serial
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("\n\nIniciando sistema...");

  // Configuração dos pinos dos relés
  for (uint8_t i = 0; i < NUM_RELAYS; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW); // Garante que inicia desligado
    Serial.printf("Relé %d configurado no pino %d\n", i+1, RELAY_PINS[i]);
  }

  // Conexão WiFi
  wifiConnected = connectWiFi();

  if (wifiConnected) {
    // Configuração dos dispositivos na Alexa
    for (uint8_t i = 0; i < NUM_RELAYS; i++) {
      espalexa.addDevice(DEVICE_NAMES[i], [i](uint8_t brightness) {
        relayCallback(brightness, i);
      });
      Serial.printf("Dispositivo '%s' registrado\n", DEVICE_NAMES[i]);
    }
    
    espalexa.begin();
    Serial.println("Servidor Espalexa iniciado");
  } else {
    Serial.println("Falha na conexão WiFi. Reiniciando em 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

// ================= LOOP PRINCIPAL ================= //
void loop() {
  espalexa.loop();
  
  // Verifica periodicamente a conexão WiFi
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 30000) { // A cada 30 segundos
    lastCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Tentando reconectar...");
      wifiConnected = connectWiFi();
    }
  }
}

// ================= FUNÇÕES ================= //

// Função para conexão WiFi com tratamento robusto
bool connectWiFi() {
  Serial.println("\nConectando ao WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado com sucesso!");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
    return true;
  } else {
    Serial.println("\nFalha na conexão WiFi");
    return false;
  }
}

// Callback genérico para todos os relés
void relayCallback(uint8_t brightness, uint8_t relayNum) {
  if (relayNum >= NUM_RELAYS) return;
  
  bool state = (brightness > 0);
  digitalWrite(RELAY_PINS[relayNum], state ? HIGH : LOW);
  
  Serial.printf("Relé %d (%s) %s\n", 
                relayNum+1, 
                DEVICE_NAMES[relayNum],
                state ? "LIGADO" : "DESLIGADO");
}

// Função para controle manual dos relés (opcional)
void handleRelay(uint8_t relayNum, uint8_t brightness) {
  if (relayNum >= NUM_RELAYS) return;
  relayCallback(brightness, relayNum);
}
