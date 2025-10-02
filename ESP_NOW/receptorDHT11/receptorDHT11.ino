#include <WiFi.h>
#include <esp_now.h>

// Estructura que recibe (temperatura)
typedef struct struct_message {
  float temperature;
} struct_message;

struct_message incomingData;

// Estructura para enviar comando al emisor
typedef struct struct_command {
  bool activarRelay;
} struct_command;

struct_command cmdData;

// Dirección MAC del emisor (cambiar por la real del emisor)
uint8_t emisorAddress[] = {0x24, 0x6F, 0x28, 0xYY, 0xYY, 0xYY};

// Callback cuando se recibe temperatura del emisor
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingDataBytes, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

    Serial.print("Temperatura recibida: ");
    Serial.println(incomingData.temperature);

    // Lógica: activar relay si la temperatura supera 25°C
    if (incomingData.temperature > 25.0) {
      cmdData.activarRelay = true;
    } else {
      cmdData.activarRelay = false;
    }

    // Mandar comando de vuelta al emisor
    esp_now_send(emisorAddress, (uint8_t *) &cmdData, sizeof(cmdData));
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, emisorAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fallo al añadir el peer");
    return;
  }
}

void loop() {
  // No hace falta nada, todo funciona con callbacks
}
