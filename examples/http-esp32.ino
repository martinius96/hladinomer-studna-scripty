/*|---------------------------------|*/
/*|Projekt: Hladinomer              |*/
/*|Autor: Martin Chlebovec          |*/
/*|E-mail: martinius96@gmail.com    |*/
/*|Web: https://arduino.php5.sk     |*/
/*|Licencia pouzitia: MIT           |*/
/*|---------------------------------|*/

#include <WiFi.h>
#include <NewPingESP8266.h>
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

const char * ssid = "wifi_meno"; //meno wifi siete
const char * password = "wifi_heslo"; //heslo na wifi siet
const char * host = "www.arduino.php5.sk"; //bez https a bez www
const int httpPort = 80;

WiFiClient client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi pripojene s IP:");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  int vzdialenost = sonar.ping_cm();
  delay(50);
  if (vzdialenost > 0) {
    vzdialenost = 0;
    for (int i = 0; i < 5; i++) {
      vzdialenost += sonar.ping_cm();
      delay(50);
    }
    vzdialenost = vzdialenost / 5;
    Serial.print("Vzdialenost medzi senzorom a predmetom je: ");
    Serial.print(vzdialenost);
    Serial.println(" cm.");
    if (client.connect(host, httpPort)) {
      String hodnota = String(vzdialenost);
      Serial.println("Pripojenie uspesne na webserver, vykonavam request... ");
      String url = "/studna/data.php?hodnota=" + hodnota;
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP32\r\n" + "Connection: close\r\n\r\n");
      Serial.println("Hodnoty do databazy uspesne odoslane");
      client.stop();
    } else {
      Serial.println("Pripojenie zlyhalo...");
    }
  }
  else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
    delay(500);
  }
  for (int i = 0; i <= 300; i++) {
    delay(1000);
  }
}
