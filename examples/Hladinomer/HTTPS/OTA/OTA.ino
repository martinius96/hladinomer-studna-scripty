/*|------------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTPS - OTA - HC-SR04 / JSN-SR04T / HY-SRF05                  |*/
/*|ESP8266 (NodeMCU, Wemos D1 Mini, Generic), ESP32 (DevKit, Generic)                  |*/
/*|Autor: Bc. Martin Chlebovec (martinius96)                                           |*/
/*|E-mail: martinius96@gmail.com                                                       |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/  |*/
/*|Testovacie webove rozhranie pre HTTP: http://arduino.clanweb.eu/studna_s_prekladom/ |*/
/*|Testovacie webove rozhranie pre HTTPS: https://esp32.sk/studna_s_prekladom/         |*/
/*|Knižnica ESP8266NewPing je dostupná v Github repozitári:                            |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť      |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/     |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                                |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv     |*/
/*|Licencia pouzitia: MIT                                                              |*/
/*|Revízia: 5. April 2021                                                              |*/
/*|------------------------------------------------------------------------------------|*/

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NewPingESP8266.h>
#include <WiFiClientSecure.h>
const char* host = "esp32.sk"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie s HTTPS konektivitou data odosielaju na: esp32.sk/studna_s_prekladom/data.php (HTTP POST ONLY)
const char * ssid = "WIFI_NAME"; //MENO WiFi SIETE
const char * password = "WIFI_PASSWORD"; //HESLO WiFi SIETE

unsigned long timer2 = 0;
#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#include "esp_system.h"
#define pinTrigger    22
#define pinEcho       23
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

void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <NewPingESP8266.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
const char fingerprint[] PROGMEM = "‎02 5b 13 b3 a6 d8 4c f3 b1 21 58 aa ab 3f 9f 69 2e 27 14 66"; //https://esp32.sk/studna_s_prekladom/
#endif
WiFiClientSecure client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  ArduinoOTA.onStart([]() {
    Serial.println(F("Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println(F(""));
  Serial.println(F("Wifi pripojene s IP:"));
  Serial.println(WiFi.localIP());
#if defined(ESP32)
  client.setCACert(test_root_ca);
  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
#elif defined(ESP8266)
  //client.setFingerprint(fingerprint); //DOCASNE
  client.setInsecure();
  ESP.wdtEnable(30000);
#endif
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
#if defined(ESP32)
  timerWrite(timer, 0); //reset timer (feed watchdog)
#elif defined(ESP8266)
  ESP.wdtFeed();
#endif
  ArduinoOTA.handle();
  yield();
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
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println(F("User-Agent: ESP"));
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
        timer2 = 0; //reset pocitadla, vykoname opakovany request
      }
    }
    else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      delay(500);
      timer2 = 0; //reset pocitadla, vykoname opakovany request
    }
    client.stop();
  }
}
