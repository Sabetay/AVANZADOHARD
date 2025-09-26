#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  float temperature;
} struct_message;

struct_message incomingData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  Serial.print("Temperatura recibida: ");
  Serial.println(incomingData.temperature);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nada, solo espera mensajes
}
