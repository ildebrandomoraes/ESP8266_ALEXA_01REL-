#include <ESP8266WiFi.h>
#include <Espalexa.h>

// Configuração do relé
#define RELAY_PIN 0  // GPIO0 para ESP-01 (verificar compatibilidade com seu hardware)
const char* DEVICE_NAME = "Meu Rele";  // Nome que aparecerá na Alexa

// Bytes de comando do relé (conforme sua amostra)
uint8_t relayOn[] = {0xA0, 0x01, 0x00, 0xA1};
uint8_t relayOff[] = {0xA0, 0x01, 0x01, 0xA2};

// Configuração da rede Wi-Fi
const char* WIFI_SSID = "ildebrando";
const char* WIFI_PASSWORD = "plymount1960.";

// Variáveis globais
Espalexa espalexa;
bool wifiConnected = false;
const unsigned long SERIAL_BAUDRATE = 115200;

// Protótipos de função
bool connectWiFi();
void relayCallback(uint8_t brightness);
void sendRelayCommand(bool state);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("\nIniciando sistema de controle de relé...");

  // Configuração do pino do relé
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Inicia desligado (HIGH para módulo relé ativo em LOW)
  Serial.println("Relé configurado no pino GPIO0");

  // Conexão WiFi
  wifiConnected = connectWiFi();

  if (wifiConnected) {
    // Configuração do dispositivo na Alexa
    espalexa.addDevice(DEVICE_NAME, relayCallback);
    Serial.printf("Dispositivo '%s' registrado na Alexa\n", DEVICE_NAME);
    
    espalexa.begin();
    Serial.println("Servidor Espalexa iniciado");
  } else {
    Serial.println("Falha na conexão WiFi. Reiniciando em 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

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

// Função para conexão WiFi
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
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nFalha na conexão WiFi");
    return false;
  }
}

// Callback para controle do relé pela Alexa
void relayCallback(uint8_t brightness) {
  bool state = (brightness > 0);
  digitalWrite(RELAY_PIN, state ? LOW : HIGH); // Inverte a lógica (LOW ativa o relé)
  sendRelayCommand(state);
  
  Serial.printf("Relé %s\n", state ? "LIGADO" : "DESLIGADO");
}

// Envia comando serial para o relé
void sendRelayCommand(bool state) {
  Serial.write(state ? relayOn : relayOff, 4);
  Serial.printf(" [Serial] Comando %s enviado\n", state ? "LIGAR" : "DESLIGAR");
}
