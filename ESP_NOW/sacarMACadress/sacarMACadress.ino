#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   // Ponemos el WiFi en modo estación
 
}

void loop() {
  Serial.println("Dirección MAC de la ESP32:");
  Serial.println(WiFi.macAddress());
  delay (7000);
}
