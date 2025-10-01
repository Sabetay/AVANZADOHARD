#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  float temperature;
} struct_message;

struct_message incomingData;

// Callback cuando se reciben datos
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingDataBytes, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

    Serial.print("Mensaje recibido de: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", info->src_addr[i]);
      if (i < 5) Serial.print(":");
    }
    Serial.println();

    Serial.print("Temperatura recibida: ");
    Serial.println(incomingData.temperature);
  } else {
    Serial.println("Tamaño de datos incorrecto recibido.");
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar modo WiFi
  WiFi.mode(WIFI_STA);
  Serial.print("MAC del receptor: ");
  Serial.println(WiFi.macAddress());

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  // Registrar callback de recepción
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No hace falta nada acá, solo escucha
}
