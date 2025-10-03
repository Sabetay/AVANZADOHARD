#include <WiFi.h>
#include <esp_now.h>

// Estructura para enviar comando al emisor
typedef struct struct_command {
  bool activarRelay;
} struct_command;

struct_command cmdData;

// Dirección MAC del emisor (cambiar por la real)
uint8_t emisorAddress[] = {0xA0, 0xA3, 0xB3, 0x29, 0x59, 0xA8};

// Pin del botón
#define BUTTON_PIN 14  

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Botón con resistencia interna

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, emisorAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fallo al añadir el peer");
    return;
  }
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {  
    // Botón apretado → encender relay
    cmdData.activarRelay = true;
    esp_now_send(emisorAddress, (uint8_t *) &cmdData, sizeof(cmdData));
    Serial.println("Botón presionado → Enviar comando ENCENDER relay");
    delay(500); // Anti-rebote
  } else {
    // Botón suelto → apagar relay
    cmdData.activarRelay = false;
    esp_now_send(emisorAddress, (uint8_t *) &cmdData, sizeof(cmdData));
    Serial.println("Botón liberado → Enviar comando APAGAR relay");
    delay(500);
  }
}
