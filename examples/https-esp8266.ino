/*|---------------------------------|*/
/*|Projekt: Hladinomer              |*/
/*|Autor: Martin Chlebovec          |*/
/*|E-mail: martinius96@gmail.com    |*/
/*|Web: https://arduino.php5.sk     |*/
/*|Licencia pouzitia: MIT           |*/
/*|Arduino Core 2.5.0+              |*/
/*|---------------------------------|*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

const char * ssid = "wifi_meno"; //meno wifi siete
const char * password = "wifi_heslo"; //heslo na wifi siet
const char * host = "arduino.php5.sk"; //bez https a bez www

const char fingerprint[] PROGMEM = "b0 6d 7f 8c 98 78 8e 6e 0a 57 a8 2f 7e d1 40 2a 1e 3f 48 f7";
const int httpsPort = 443; //https port
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
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    if (client.connect(host, httpsPort)) {
      String hodnota = String(vzdialenost);
      Serial.println("Pripojenie uspesne na webserver, vykonavam request... ");
      String url = "/studna/data.php?hodnota=" + hodnota;
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
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
