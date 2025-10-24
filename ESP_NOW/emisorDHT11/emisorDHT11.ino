#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define RELAY_PIN 5

DHT dht(DHTPIN, DHTTYPE);

// Estructura de datos a enviar (temperatura)
typedef struct struct_message {
  float temperature;
} struct_message;

struct_message myData;

// Estructura para recibir comando desde el receptor
typedef struct struct_command {
  bool activarRelay;
} struct_command;

struct_command incomingCmd;

// Dirección MAC del receptor (manteniendo la actual)
uint8_t receptorAddress[] = {0xC8, 0xF0, 0x9E, 0xF7, 0xF3, 0x5C};

// Callback al enviar datos
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("Estado del envío: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Exitoso" : "Fallido");
}

// Callback al recibir comandos desde el receptor
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(incomingCmd)) {
    memcpy(&incomingCmd, incomingData, sizeof(incomingCmd));
    Serial.print("Comando recibido - Activar Relay: ");
    Serial.println(incomingCmd.activarRelay);

    if (incomingCmd.activarRelay) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay encendido!");
    } else {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay apagado!");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  dht.begin();
  WiFi.mode(WIFI_STA);
  Serial.print("MAC del EMISOR: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receptorAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fallo al añadir el peer");
    return;
  }
}

void loop() {
  float temp = dht.readTemperature();
  if (!isnan(temp)) {
    myData.temperature = temp;
    esp_now_send(receptorAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.print("Temperatura enviada: ");
    Serial.println(temp);
  } else {
    Serial.println("Error leyendo temperatura");
  }

  delay(7000);
}
