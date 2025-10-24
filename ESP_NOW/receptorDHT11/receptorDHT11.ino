#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>

// Estructura para enviar comando al emisor
typedef struct struct_command {
  bool activarRelay;
} struct_command;

struct_command cmdData;

// Estructura para recibir temperatura desde el emisor
typedef struct struct_message {
  float temperature;
} struct_message;

struct_message incomingData;

// Dirección MAC del emisor (cambiar por la real)
uint8_t emisorAddress[] = {0xA0, 0xA3, 0xB3, 0x29, 0x59, 0xA8};

// Servidor web en el puerto 80
WebServer server(80);

float temperaturaActual = 0.0;
bool relayEstado = false;

// Callback cuando se recibe un mensaje (temperatura)
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingDataBytes, int len) {
  if (len == sizeof(incomingData)) {
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
    temperaturaActual = incomingData.temperature;
    Serial.print("Temperatura recibida: ");
    Serial.println(temperaturaActual);
  }
}

// Función para enviar comando al emisor
void enviarComando(bool activar) {
  cmdData.activarRelay = activar;
  esp_now_send(emisorAddress, (uint8_t *)&cmdData, sizeof(cmdData));
  relayEstado = activar;
  Serial.print("Comando enviado → Relay ");
  Serial.println(activar ? "ENCENDIDO" : "APAGADO");
}

// Página principal
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="es">
  <head>
    <meta charset="UTF-8">
    <title>Control del Relay y Temperatura</title>
    <style>
      body {
        background-color: #f0f4f8;
        font-family: Arial, sans-serif;
        text-align: center;
        margin-top: 50px;
      }
      h1 {
        color: #333;
      }
      .temp {
        font-size: 2em;
        color: #0066cc;
        margin-bottom: 20px;
      }
      button {
        font-size: 1.2em;
        padding: 12px 30px;
        margin: 10px;
        border: none;
        border-radius: 10px;
        cursor: pointer;
        background-color: #008CBA;
        color: white;
        transition: 0.3s;
      }
      button:hover {
        background-color: #005f73;
      }
      .on {
        background-color: #4CAF50;
      }
      .off {
        background-color: #f44336;
      }
    </style>
    <script>
      function toggleRelay(state) {
        fetch('/relay?state=' + state);
      }
      function actualizarTemp() {
        fetch('/temperature')
          .then(response => response.text())
          .then(data => {
            document.getElementById('tempValue').innerText = data + ' °C';
          });
      }
      setInterval(actualizarTemp, 5000);
      window.onload = actualizarTemp;
    </script>
  </head>
  <body>
    <h1>Panel de Control ESP32</h1>
    <div class="temp">Temperatura: <span id="tempValue">--</span></div>
    <button class="on" onclick="toggleRelay(1)">Encender Relay</button>
    <button class="off" onclick="toggleRelay(0)">Apagar Relay</button>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// Endpoint para manejar encendido/apagado del relay
void handleRelay() {
  if (server.hasArg("state")) {
    String state = server.arg("state");
    if (state == "1") {
      enviarComando(true);
      server.send(200, "text/plain", "Relay encendido");
    } else {
      enviarComando(false);
      server.send(200, "text/plain", "Relay apagado");
    }
  } else {
    server.send(400, "text/plain", "Falta parámetro 'state'");
  }
}

// Endpoint para devolver la temperatura actual
void handleTemperature() {
  server.send(200, "text/plain", String(temperaturaActual, 1));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Iniciar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, emisorAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Fallo al añadir el peer");
    return;
  }

WiFi.begin("IoTB", "inventaronelVAR");   // Poné acá tu red WiFi

Serial.print("Conectando a WiFi...");
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
Serial.println("");
Serial.print("Conectado! IP: ");
Serial.println(WiFi.localIP());

  // Rutas del servidor web
  server.on("/", handleRoot);
  server.on("/relay", handleRelay);
  server.on("/temperature", handleTemperature);

  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();
}
