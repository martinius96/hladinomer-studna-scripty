/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - Compact - HC-SR04 / JSN-SR04T / HY-SRF05              |*/
/*|Arduino + Ethernet (W5100 / W5500, ENC28J60), ESP8266 (NodeMCU), ESP32 (DevKit)    |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie: http://arduino.clanweb.eu/studna_s_prekladom/         |*/
/*|Knižnice NewPing, ESP8266NewPing a Ethernet2 je dostupná v Github repozitári:      |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť     |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/    |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                               |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv    |*/
/*|Licencia pouzitia: MIT                                                             |*/
/*|Revízia: 24. Jun 2022                                                              |*/
/*|-----------------------------------------------------------------------------------|*/

const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie sa data odosielaju na: arduino.clanweb.eu/studna_s_prekladom/data.php (HTTP POST ONLY)
#define maxVzdialenost 450
unsigned long timer2 = 0;
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//HLAVICKOVE SUBORY PRE ARDUINO A ETHERNET SHIELD + watchdog
#include <avr\wdt.h>
#include <SPI.h>
#include <Ethernet.h> //Ethernet shield Wiznet W5100 - zakomentovat ak sa nepouziva
//#include <Ethernet2.h> //Ethernet modul Wiznet W5500 - zakomentovat ak sa nepouziva
//#include <Ethernet3.h> //Ethernet modul WIZ850io / USR-ES1 (Wiznet W5500 V2) - zakomentovat ak sa nepouziva
//#include <UIPEthernet.h> //Ethernet modul ENC28J60 - zakomentovat ak sa nepouziva
#include <NewPing.h>
#define pinTrigger    5
#define pinEcho       6
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
byte mac[] = { 0xAA, 0xBB, 0xCA, 0xDC, 0xEE, 0xDE };
//IPAddress ip(192, 168, 0, 2);
//IPAddress dnServer(192, 168, 0, 1);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);
EthernetClient client;
#endif
#if defined(ESP32) || defined(ESP8266)
#include <NewPingESP8266.h>
const char * ssid = "WIFI_MENO_SIETE"; //MENO WiFi SIETE
const char * password = "WIFI_HESLO_SIETE"; //HESLO WiFi SIETE
#endif
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
#endif
#if defined(ESP32)
#include <WiFi.h>
#include "esp_system.h"
#define pinTrigger    22 //D22
#define pinEcho       23 //D23
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
#endif
void setup() {
  Serial.begin(115200);
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Konfigurujem Ethernet adapter"));
    Ethernet.begin(mac); // pre DHCP
    //Ethernet.begin(mac, ip); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns, gateway); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns, gateway, subnet); //pre staticku IPv4
    //IBA PRE Ethernet3.h !!! POZOR --> INY BEGIN ORDER PARAMETROV.... : //Ethernet.begin(mac, ip, subnet, gateway, dns);
  }
  Serial.print(F("Priradena IP: "));
  Serial.println(Ethernet.localIP());
  wdt_enable(WDTO_8S); //watchdog on 8 sec trigger
#elif defined(ESP32) || defined(ESP8266)
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
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
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  wdt_reset(); //Feed watchdog
#endif
#if defined(ESP32) || defined(ESP8266)
  yield();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
#endif
#if defined(ESP8266)
  ESP.wdtFeed(); //feed watchdog
#endif
#if defined(ESP32)
  timerWrite(timer, 0); //reset timer (feed watchdog)
#endif
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
        client.println(F("User-Agent: MCU"));
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
}
