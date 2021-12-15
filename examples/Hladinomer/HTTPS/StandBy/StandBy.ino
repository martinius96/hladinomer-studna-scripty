/*|------------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTPS - Compact - HC-SR04 / JSN-SR04T / HY-SRF05              |*/
/*|ESP8266 (NodeMCU, Wemos D1 Mini), ESP32 (DevKit, Standalone)                        |*/
/*|Autor: Martin Chlebovec (martinius96)                                               |*/
/*|E-mail: martinius96@gmail.com                                                       |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/  |*/
/*|Testovacie webove rozhranie pre HTTPS: https://hladinomer.000webhostapp.com/        |*/
/*|Knižnice NewPing, ESP8266NewPing a Ethernet2 je dostupná v Github repozitári:       |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť      |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/     |*/
/*|Na toto webove rozhranie posiela mikrokontroler data iba po HTTPS                   |*/
/*|Pre Arduino + Ethernet HTTPS nie je podporované! Možno využiť len HTTP variant!     |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv     |*/
/*|Licencia pouzitia: MIT                                                              |*/
/*|Revízia: 15. December 2021                                                          |*/
/*|------------------------------------------------------------------------------------|*/

const char* host = "hladinomer.000webhostapp.com"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
const char * ssid = "WIFI_NAME"; //MENO WiFi SIETE

const char * password = "WIFI_PASSWORD"; //HESLO WiFi SIETE



//Root CA cert --> CERTIFIKÁT CERTIFIKAČNEJ AUTORITY, KTORÁ VYDALA CERTIFIKÁT VÁŠ WEBSERVER v .pem formáte
//DST ROOT CA X3 EXAMPLE (https://i.imgur.com/fvw4huT.png)
const static char* test_root_ca PROGMEM = \
    "-----BEGIN CERTIFICATE-----\n" \
    "CHANGE IT TO YOUR ROOT CA cert in .pem\n" \
    "-----END CERTIFICATE-----\n";   



#include <WiFiClientSecure.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
X509List cert(test_root_ca);

#elif defined(ESP32)
#include <WiFi.h>
#include <NewPingESP8266.h>
#include "esp_system.h"
#define pinTrigger    22 //D22
#define pinEcho       23 //D23
#define maxVzdialenost 450
const int wdtTimeout = 30000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}
#endif
WiFiClientSecure client;
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
unsigned long timer2 = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("Wifi pripojene s IP:"));
  Serial.println(WiFi.localIP());
#if defined(ESP8266)
  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  Serial.printf("Using certificate: %s\n", test_root_ca);
  client.setTrustAnchors(&cert);
  ESP.wdtEnable(30000); //30s SW watchdog
#elif defined(ESP32)
  client.setCACert(test_root_ca);
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt

#endif
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
#if defined(ESP8266)
  while (WiFi.status() != WL_CONNECTED) {
    ESP.wdtFeed(); //feed watchdog
    delay(500);
    Serial.print(F("."));
  }
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
      String data = "hodnota=" + String(vzdialenost) + "&token=123456789";
      if (client.connect(host, 443)) {
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
  while (WiFi.status() != WL_CONNECTED) {
    timerWrite(timer, 0); //reset timer (feed watchdog)
    delay(500);
    Serial.print(F("."));
  }
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
      if (client.connect(host, 443)) {
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
