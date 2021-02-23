/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - Compact - HC-SR04 / JSN-SR04T / HY-SRF05              |*/
/*|Arduino + Ethernet (W5100 / W5500, ENC28J60), ESP8266 (NodeMCU), ESP32 (DevKit)    |*/
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
/*|Revízia: 19. Februar 2021                                                          |*/
/*|-----------------------------------------------------------------------------------|*/

const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie sa data odosielaju na: arduino.clanweb.eu/studna_s_prekladom/data.php (HTTP POST ONLY)

//Kompatibilne mikrokontrolery z rady Arduino: Uno, Nano, Mega (R3)
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
//HLAVICKOVE SUBORY PRE ARDUINO A ETHERNET SHIELD + watchdog
#include <avr\wdt.h>
#include <SPI.h>



//VOLBA ETHERNET MODULU / SHIELDU K ARDUINU Z RADY WIZNET / ENC (Microchip)
///////////////////////////////////////////////////////////////////////////////////////
#include <Ethernet.h> //Ethernet shield Wiznet W5100 - zakomentovat ak sa nepouziva
//#include <Ethernet2.h> //Ethernet modul Wiznet W5500 - zakomentovat ak sa nepouziva
//#include <UIPEthernet.h> //Ethernet modul ENC28J60 - zakomentovat ak sa nepouziva
///////////////////////////////////////////////////////////////////////////////////////



//PREMENNE, HLAVICKOVY SUBOR, OBJEKT PRE HC-SR04 / JSN-SR04T
#include <NewPing.h>
#define pinTrigger    5
#define pinEcho       6
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);

byte mac[] = { 0xAA, 0xBB, 0xCA, 0xDC, 0xEE, 0xDD };
//IPAddress ip(192, 168, 0, 2);
//IPAddress dnServer(192, 168, 0, 1);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);
unsigned long timer = 0;
EthernetClient client;

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

#elif defined(ESP32)
#include <WiFi.h>
#include <NewPingESP8266.h>
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
const char * ssid = "WIFI_MENO"; //meno wifi siete
const char * password = "WIFI_HESLO"; //heslo na wifi siet
WiFiClient client;
unsigned long timer2 = 0;
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
  }
  Serial.print(F("Priradena IP: "));
  Serial.println(Ethernet.localIP());
  wdt_enable(WDTO_8S); //watchdog on 8 sec trigger
#elif defined(ESP32) || defined(ESP8266)
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
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
  if (Ethernet.begin(mac) == 0) {
    Ethernet.begin(mac); // pre DHCP
    //Ethernet.begin(mac, ip); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns, gateway); //pre staticku IPv4
    //Ethernet.begin(mac, ip, dns, gateway, subnet);
  }

  if ((millis() - timer) >= 300000 || timer == 0) { //rutina raz za 5 minut
    timer = millis();
    Ethernet.maintain(); //pre DHCP, pri statickej IP zakomentovat!
    int vzdialenost = sonar.ping_cm();
    delay(50);
    if (vzdialenost > 0) {
      vzdialenost = 0;
      for (int i = 0; i < 10; i++) {
        vzdialenost += sonar.ping_cm();
        delay(50);
      }
      wdt_reset();
      vzdialenost = vzdialenost / 10;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(F(" cm."));
      client.stop(); //ukoncenie vsetkych existujucich spojeni
      String data = "hodnota=" + String(vzdialenost);
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println(F("User-Agent: EthernetW5100")); //Moznost pouzit iny USER agent
        client.println(F("Connection: close"));
        client.println(F("Content-Type: application/x-www-form-urlencoded;"));
        client.print(F("Content-Length: "));
        client.println(data.length());
        client.println();
        client.println(data);
        Serial.println(F("Data uspesne odoslane na webove rozhranie"));
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
      }
      client.stop();
    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      delay(500);
      timer = 0; //vynulujeme timer, znovu nameriame a vykoname request
    }
  }
#endif
#if defined(ESP32) || defined(ESP8266)
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
#endif
#if defined(ESP8266)
  while (WiFi.status() != WL_CONNECTED) {
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
      client.stop();
      String data = "hodnota=" + String(vzdialenost);
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
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
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
      String data = "hodnota=" + String(vzdialenost);
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
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
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
