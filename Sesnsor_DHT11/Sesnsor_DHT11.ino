# include <DHT.h>

DHT dht (4, DHT11); //ponemos DHT11 pq ese es el modelo del sensor
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  delay(3000);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  float Temperatura = dht.readTemperature();
  float Humedad = dht.readHumidity();
  Serial.print("Temp: "); Serial.print(Temperatura); Serial.print("°C");
  Serial.print("Humedad: "); Serial.print(Humedad);

}
