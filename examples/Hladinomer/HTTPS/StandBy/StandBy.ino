/*|------------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTPS - Compact - HC-SR04 / JSN-SR04T / HY-SRF05              |*/
/*|ESP8266 (NodeMCU, Wemos D1 Mini), ESP32 (DevKit, Standalone)                        |*/
/*|Autor: Bc. Martin Chlebovec (martinius96)                                           |*/
/*|E-mail: martinius96@gmail.com                                                       |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/  |*/
/*|Testovacie webove rozhranie pre HTTP: http://arduino.clanweb.eu/studna_s_prekladom/ |*/
/*|Testovacie webove rozhranie pre HTTPS: https://esp32.sk/studna_s_prekladom/         |*/
/*|Knižnice NewPing, ESP8266NewPing a Ethernet2 je dostupná v Github repozitári:       |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť      |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/     |*/
/*|Na toto webove rozhranie posiela mikrokontroler data iba po HTTPS                   |*/
/*|Pre Arduino + Ethernet HTTPS nie je podporované! Možno využiť len HTTP variant!     |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv     |*/
/*|Licencia pouzitia: MIT                                                              |*/
/*|Revízia: 1. Apríl 2021                                                              |*/
/*|------------------------------------------------------------------------------------|*/

const char* host = "esp32.sk"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie s HTTPS konektivitou data odosielaju na: esp32.sk/studna_s_prekladom/data.php (HTTP POST ONLY)
const char * ssid = "WIFI_MENO_SIETE"; //MENO WiFi SIETE
const char * password = "WIFI_HESLO_SIETE"; //HESLO WiFi SIETE

#include <WiFiClientSecure.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
const char fingerprint[] PROGMEM = "‎02 5b 13 b3 a6 d8 4c f3 b1 21 58 aa ab 3f 9f 69 2e 27 14 66"; //https://esp32.sk/studna_s_prekladom/
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

//esp32.sk Root CA (vlastná OWN CA, self-signed).
const static char* test_root_ca PROGMEM = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDTzCCAjcCFGoeGOBevdCzZX61CSkNsw1yLOgeMA0GCSqGSIb3DQEBCwUAMGQx\n" \
    "CzAJBgNVBAYTAlVTMRMwEQYDVQQIDApGYWtlIFN0YXRlMRYwFAYDVQQHDA1GYWtl\n" \
    "IExvY2FsaXR5MRUwEwYDVQQKDAxGYWtlIENvbXBhbnkxETAPBgNVBAMMCGVzcDMy\n" \
    "LnNrMB4XDTE4MTIxNDE4MTcyNVoXDTIzMTIxMzE4MTcyNVowZDELMAkGA1UEBhMC\n" \
    "VVMxEzARBgNVBAgMCkZha2UgU3RhdGUxFjAUBgNVBAcMDUZha2UgTG9jYWxpdHkx\n" \
    "FTATBgNVBAoMDEZha2UgQ29tcGFueTERMA8GA1UEAwwIZXNwMzIuc2swggEiMA0G\n" \
    "CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDeimA7p/eIaGtXPM2QyD3+tDI+6ltB\n" \
    "bPzHUPupBK3EYi1bCtHGg0Lq8cbCdFVhJJnvRGoIdz/knhTIkMphMvTC5UJLooPx\n" \
    "RnI9KSdXWs2n/wXYvPS9Txthz6zv0Ua9Dp+OcXvz0wzG1skhd+RPAm9zUJDJ42P1\n" \
    "FIjcehxH93GgrHzXz6zklFt01/8IAcBt7gl8/qBnjJNA8SS0BoYgfRnNOvn6k1yk\n" \
    "ZZtyfIp68NWlfdveMd8Ig2pUIuvTh7MyUFjumcDLqAs2J+SenUYHEcfZJK4Vmdfo\n" \
    "bZam4hx8f0BGrxqDhDMuTmh/Jqa8ZYANfR7ctD1DU6u7aceGYmSgCiNpAgMBAAEw\n" \
    "DQYJKoZIhvcNAQELBQADggEBACA5n68AsjspvR3N33jF83FZqDXuvQbmY7lBm2nN\n" \
    "Yh2265f9mRGrOO3z6lHKUfs5BJXxxahAKPQ07H9zQuYvIW0y8Jcgs4FlFhAuqgss\n" \
    "KWuqJicf/noRPq/MC7mTLSGVrCo3R3x8U842j/Jdi0Ygb7AgFMp5F7DoS88QEMuK\n" \
    "2UbJSA0czc24NImg9Y8Ve6VS4m+lsjsl77L15JCuDeZ7UBegmYoDmEIC/MohSD5t\n" \
    "goX0+fCH4EsIkd6Zh8P6GTlRCglyGrSFgKTV/6HPSptAFvMFUofnfedCYJXrkjKc\n" \
    "psVaEdVBIz/a4jcB6mkQQm06+KusHmwsc/VNYLD6+pjmIE0=\n" \
    "-----END CERTIFICATE-----\n";
WiFiClientSecure client;

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}
#endif
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
    delay(500);
    Serial.print(F("."));
  }
  ESP.wdtFeed(); //feed watchdog
  yield(); //nechaj ESP dokoncit WiFi stack
  if ((millis() - timer2) >= 300000 || timer2 == 0) {
    timer2 = millis();
    int vzdialenost = sonar.ping_cm();
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
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
