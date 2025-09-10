// Ejemplo básico ESP32 - Hello World + LED parpadeando

#define LED_BUILTIN 2  

void setup() {

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Hello World");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(500); 
}