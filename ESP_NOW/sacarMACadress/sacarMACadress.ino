#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   // Ponemos el WiFi en modo estación
  Serial.println("Dirección MAC de la ESP32:");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // No hace falta nada en el loop
}
