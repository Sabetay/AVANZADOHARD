#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4      // Pin del DHT11
#define DHTTYPE DHT11 // Modelo del sensor

DHT dht(DHTPIN, DHTTYPE);

// Dirección MAC del receptor (ESP32 #2) → cambiar según tu dispositivo
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF};

typedef struct struct_message {
  float temperature;
} struct_message;

struct_message myData;

void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Mensaje enviado: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Éxito" : "Fallo");
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnSent);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
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

  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  Serial.print("Temperatura enviada: ");
  Serial.println(myData.temperature);

  delay(3000); // cada 3 segundos
}
