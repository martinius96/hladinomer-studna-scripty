/*|-----------------------------------------------  --|*/
/*|Projekt: Hladinomer - ESP32 - HTTP                 |*/
/*|Autor: Martin Chlebovec                            |*/
/*|E-mail: martinius96@gmail.com                      |*/
/*|Web: http://arduino.clanweb.eu/studna_s_prekladom/ |*/
/*|Licencia pouzitia: MIT                             |*/
/*|Revízia: 13. Februar 2021                          |*/
/*|---------------------------------------------------|*/

#include <WiFi.h>
#include <NewPingESP8266.h>
#include "esp_system.h"
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

const char * ssid = "wifi_meno"; //meno wifi siete
const char * password = "wifi_heslo"; //heslo na wifi siet
const char * host = "arduino.clanweb.eu"; //bez https a bez www
unsigned long timer2 = 0;
const int wdtTimeout = 30000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;
WiFiClient client;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

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
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
  while (WiFi.status() != WL_CONNECTED) {
    timerWrite(timer, 0); //reset timer (feed watchdog)
    delay(500);
    Serial.print(".");
  }
  timerWrite(timer, 0); //reset timer (feed watchdog)
  yield();
  if ((millis() - timer2) >= 300000 || timer2 == 0) {
    timer2 = millis();
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
      client.stop();
      String data = "hodnota=" + String(vzdialenost);
      String url = "/studna_s_prekladom/data.php";
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println("User-Agent: ESP32");
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
      delay(500);
      client.stop();
    }
  }
}
