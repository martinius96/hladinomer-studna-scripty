/*|-------------------------------------------------|*/
/*|Projekt: Hladinomer - ESP8266 - HTTP             |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Web: https://arduino.php5.sk                     |*/
/*|Licencia pouzitia: MIT                           |*/
/*|Revízia: 26. Marec 2020                          |*/
/*|-------------------------------------------------|*/

#include <ESP8266WiFi.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);


const char * ssid = "wifi_meno"; //meno wifi siete
const char * password = "wifi_heslo"; //heslo na wifi siet
const char * host = "arduino.clanweb.eu"; //bez https a bez www

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
    String data = "hodnota=" + String(vzdialenost);
    String url = "/studna_s_prekladom/data.php";
    if (client.connect(host, 80)) {
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
    for (int i = 0; i <= 300; i++) {
      delay(1000);
    }
  }
  else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
    delay(500);
  }
}