#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

#define DHTPIN 4     
#define DHTTYPE DHT11
#define RELAY_PIN 5   // Pin donde conectas el relay

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

// Dirección MAC del receptor (cambiar por la real del receptor)
uint8_t receptorAddress[] = {0x24, 0x6F, 0x28, 0xXX, 0xXX, 0xXX};

// Callback cuando se envía
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estado del envio: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Exitoso" : "Fallido");
}

// Callback cuando se recibe
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len == sizeof(incomingCmd)) {
    memcpy(&incomingCmd, incomingData, sizeof(incomingCmd));
    Serial.print("Comando recibido - Activar Relay: ");
    Serial.println(incomingCmd.activarRelay);

    if (incomingCmd.activarRelay) {
      digitalWrite(RELAY_PIN, HIGH);  // Enciende relay
      Serial.println("Relay encendido!");
    } else {
      digitalWrite(RELAY_PIN, LOW);   // Apaga relay
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

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo;
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
    esp_now_send(receptorAddress, (uint8_t *) &myData, sizeof(myData));
    Serial.print("Temperatura enviada: ");
    Serial.println(temp);
  }
  delay(3000);
}
