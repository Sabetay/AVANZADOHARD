#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4       // Pin del DHT11
#define DHTTYPE DHT11  // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);

// 👉 Cambia esta MAC por la del receptor (ESP32 #2)
uint8_t receiverMac[] = {0xA0, 0xA3, 0xB3, 0x29, 0x59, 0xA8};

// Estructura de datos a enviar
typedef struct struct_message {
  float temperature;
} struct_message;

struct_message myData;

// Callback de confirmación de envío
void OnSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("Mensaje enviado a: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->des_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" -> ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Fallo");
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Configurar WiFi en modo estación
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  // Registrar callback de envío
  esp_now_register_send_cb(OnSent);

  // Configurar peer (receptor)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el receptor");
    return;
  }
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Error leyendo el DHT11");
    delay(2000);
    return;
  }

  myData.temperature = temp;

  // Enviar datos al receptor
  esp_now_send(receiverMac, (uint8_t *)&myData, sizeof(myData));

  Serial.print("Temperatura enviada: ");
  Serial.println(myData.temperature);

  delay(3000); // Enviar cada 3 segundos
}

