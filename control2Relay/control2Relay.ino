#include <WiFi.h>
#include <WebServer.h>

// Pines de los relés según la imagen
#define RELAY1 22
#define RELAY2 23

// Configuración WiFi
const char* ssid = "IoTB";
const char* password = "inventaronelVAR";

// Crear servidor en puerto 80
WebServer server(80);

// Página principal con botones
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Control de Relés ESP32</title></head>";
  html += "<body><h1>Control de Módulo de 2 Relés</h1>";

  // Botones para el Relay 1
  html += "<h2>Relay 1</h2>";
  html += "<p><a href='/relay1/on'><button>Encender</button></a></p>";
  html += "<p><a href='/relay1/off'><button>Apagar</button></a></p>";

  // Botones para el Relay 2
  html += "<h2>Relay 2</h2>";
  html += "<p><a href='/relay2/on'><button>Encender</button></a></p>";
  html += "<p><a href='/relay2/off'><button>Apagar</button></a></p>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Rutas para Relay 1
void handleRelay1On() {
  digitalWrite(RELAY1, LOW);  // Activa el relé (activo en LOW)
  Serial.println("Relay 1 ON");
  handleRoot();
}

void handleRelay1Off() {
  digitalWrite(RELAY1, HIGH); // Apaga el relé
  Serial.println("Relay 1 OFF");
  handleRoot();
}

// Rutas para Relay 2
void handleRelay2On() {
  digitalWrite(RELAY2, LOW);
  Serial.println("Relay 2 ON");
  handleRoot();
}

void handleRelay2Off() {
  digitalWrite(RELAY2, HIGH);
  Serial.println("Relay 2 OFF");
  handleRoot();
}

void setup() {
  Serial.begin(115200);

  // Configuración de pines
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  // Relés apagados al inicio
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);

  // Conexión a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado a WiFi. IP: ");
  Serial.println(WiFi.localIP());

  // Configuración de rutas
  server.on("/", handleRoot);
  server.on("/relay1/on", handleRelay1On);
  server.on("/relay1/off", handleRelay1Off);
  server.on("/relay2/on", handleRelay2On);
  server.on("/relay2/off", handleRelay2Off);

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
