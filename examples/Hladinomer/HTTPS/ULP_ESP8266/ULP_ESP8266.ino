/*|---------------------------------------------------------|*/
/*|Projekt: Hladinomer - ESP8266 - HTTPS - Ultra Low Power  |*/
/*|Autor: Martin Chlebovec                                  |*/
/*|E-mail: martinius96@gmail.com                            |*/
/*|Web: https://arduino.php5.sk                             |*/
/*|Licencia pouzitia: MIT                                   |*/
/*|Revízia: 30. Jul 2020                                    |*/
/*|CORE 2.5.0 / 2.5.2 ONLY                                  |*/
/*|---------------------------------------------------------|*/

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
const char fingerprint[] PROGMEM = "00 2c c1 3a 3c fd a2 0a a3 f1 19 1a ee ee 54 72 93 56 7d 1b";

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
  int vzdialenost = sonar.ping_cm();
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
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
    String data = "hodnota=" + String(vzdialenost);
    String url = "/studna/data.php";
    if (client.connect(host, 443)) {
      client.println("POST " + url + " HTTP/1.0");
      client.println("Host: " + (String)host);
      client.println("User-Agent: ESP8266");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded;");
      client.print("Content-Length: ");
      client.println(data.length());
      client.println();
      client.println(data);
      Serial.println("Data uspesne odoslane na web");
    } else {
      Serial.println("Pripojenie zlyhalo...");
    }
    client.stop();
  }
  else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
  }
  Serial.println("I'm awake, but I'm going into deep sleep mode for 300 seconds");
  ESP.deepSleep(300e6);
}

void loop() {
}
