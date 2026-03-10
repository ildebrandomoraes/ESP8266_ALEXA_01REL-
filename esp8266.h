#include <ESP8266WiFi.h>
#include <Espalexa.h>

// Configuração do relé
#define RELAY_PIN 0  // GPIO usado para controle do relé
const char* DEVICE_NAME = "Smart Relay";  // Nome genérico do dispositivo

// Bytes de comando do relé
uint8_t relayOn[] = {0xA0, 0x01, 0x00, 0xA1};
uint8_t relayOff[] = {0xA0, 0x01, 0x01, 0xA2};

// Configuração da rede Wi-Fi (substitua pelos seus dados)
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

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
  Serial.println("\nIniciando sistema...");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relé inicia desligado

  wifiConnected = connectWiFi();

  if (wifiConnected) {
    espalexa.addDevice(DEVICE_NAME, relayCallback);
    espalexa.begin();
  } else {
    delay(5000);
    ESP.restart();
  }
}

void loop() {
  espalexa.loop();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 30000) {
    lastCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected = connectWiFi();
    }
  }
}

bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  return (WiFi.status() == WL_CONNECTED);
}

void relayCallback(uint8_t brightness) {
  bool state = (brightness > 0);
  digitalWrite(RELAY_PIN, state ? LOW : HIGH);
  sendRelayCommand(state);
}

void sendRelayCommand(bool state) {
  Serial.write(state ? relayOn : relayOff, 4);
}
