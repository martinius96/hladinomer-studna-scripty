/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - Compact - HC-SR04 / JSN-SR04T / HY-SRF05              |*/
/*|ESP8266 (NodeMCU), ESP32 (DevKit)                                                  |*/
/*|Autor: Bc. Martin Chlebovec (martinius96)                                          |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie: http://arduino.clanweb.eu/studna_s_prekladom/         |*/
/*|Knižnice NewPing, ESP8266NewPing a Ethernet2 je dostupná v Github repozitári:      |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť     |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/    |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                               |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv    |*/
/*|Licencia pouzitia: MIT                                                             |*/
/*|Revízia: 23. December 2021                                                         |*/
/*|-----------------------------------------------------------------------------------|*/

const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie sa data odosielaju na: arduino.clanweb.eu/studna_s_prekladom/data.php (HTTP POST ONLY)



unsigned long timer = 0;

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiManager.h> //v0.16 last release from tzapu
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

#elif defined(ESP32)
#include <WiFi.h>
#include <NewPingESP8266.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "esp_system.h"
#define pinTrigger    22 //D22
#define pinEcho       23 //D23
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
const int wdtTimeout = 30000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}
#endif

#if defined(ESP32) || defined(ESP8266)

WiFiClient client;
unsigned long timer2 = 0;
#endif

void setup() {
  Serial.begin(115200);
#if defined(ESP32) || defined(ESP8266)
  WiFiManager wifiManager;
  wifiManager.autoConnect("Hladinomer_AP"); //SSID --> no password
  Serial.println("");
  Serial.println(F("Wifi pripojene s IP:"));
  Serial.println(WiFi.localIP());
#endif
#if defined(ESP8266)
  ESP.wdtEnable(30000); //30s SW watchdog
#elif defined(ESP32)
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
#endif
}

void loop() {
#if defined(ESP8266)
  ESP.wdtFeed(); //feed watchdog
  yield(); //nechaj ESP dokoncit WiFi stack
  if ((millis() - timer2) >= 300000 || timer2 == 0) {
    timer2 = millis();
    int vzdialenost = sonar.ping_cm();
    delay(50);
    if (vzdialenost > 0) {
      vzdialenost = 0;
      for (int i = 0; i < 10; i++) {
        vzdialenost += sonar.ping_cm();
        delay(50);
      }
      vzdialenost = vzdialenost / 10;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(F(" cm."));
      client.stop();
      String data = "hodnota=" + String(vzdialenost) + "&token=123456789";
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println(F("User-Agent: ESP8266"));
        client.println(F("Connection: close"));
        client.println(F("Content-Type: application/x-www-form-urlencoded;"));
        client.print(F("Content-Length: "));
        client.println(data.length());
        client.println();
        client.println(data);
        Serial.println(F("Data uspesne odoslane na web"));
        while (client.connected()) {
          String line = client.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client.readStringUntil('\n');
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
        delay(500);
        timer2 = 0;  //vynulujeme timer, znovu nameriame a vykoname request
      }
      client.stop();
    }
    else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      delay(500);
      client.stop();
      timer2 = 0; //vynulujeme timer, znovu nameriame a vykoname request
    }
  }

#elif defined(ESP32)
  timerWrite(timer, 0); //reset timer (feed watchdog)
  yield(); //nechaj ESP dokoncit WiFi stack
  if ((millis() - timer2) >= 300000 || timer2 == 0) {
    timer2 = millis();
    int vzdialenost = sonar.ping_cm();
    delay(50);
    if (vzdialenost > 0) {
      vzdialenost = 0;
      for (int i = 0; i < 10; i++) {
        vzdialenost += sonar.ping_cm();
        delay(50);
      }
      vzdialenost = vzdialenost / 10;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(F(" cm."));
      client.stop();
      String data = "hodnota=" + String(vzdialenost) + "&token=123456789";
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println(F("User-Agent: ESP32"));
        client.println(F("Connection: close"));
        client.println(F("Content-Type: application/x-www-form-urlencoded;"));
        client.print(F("Content-Length: "));
        client.println(data.length());
        client.println();
        client.println(data);
        Serial.println(F("Data uspesne odoslane na web"));
        while (client.connected()) {
          String line = client.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client.readStringUntil('\n');
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
        delay(500);
        timer2 = 0;  //vynulujeme timer, znovu nameriame a vykoname request
      }
      client.stop();
    }
    else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      delay(500);
      client.stop();
      timer2 = 0; //vynulujeme timer, znovu nameriame a vykoname request
    }
  }
#endif
}
