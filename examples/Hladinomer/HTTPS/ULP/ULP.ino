/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTPS - ULP - DEEP SLEEP - HC-SR04 / JSN-SR04T / HY-SRF05    |*/
/*|ESP8266 (NodeMCU, Wemos D1 Mini, Generic), ESP32 (DevKit, Generic)                 |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie pre HTTPS: https://hladinomer.000webhostapp.com/       |*/
/*|Knižnica ESP8266NewPing je dostupná v Github repozitári:                           |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť     |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/    |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                               |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv    |*/
/*|Licencia pouzitia: MIT                                                             |*/
/*|Revízia: 15. December 2021                                                         |*/
/*|-----------------------------------------------------------------------------------|*/

//KONFIGURACNE UDAJE - webserver, WiFi adaptér, Root CA certifikát
const char* host = "hladinomer.000webhostapp.com"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
const char * ssid = "WIFI_NAME"; //MENO WiFi SIETE
const char * password = "WIFI_PASSWORD"; //HESLO WiFi SIETE


//Root CA cert --> CERTIFIKÁT CERTIFIKAČNEJ AUTORITY, KTORÁ VYDALA CERTIFIKÁT VÁŠ WEBSERVER v .pem formáte
//DST ROOT CA X3 EXAMPLE (https://i.imgur.com/fvw4huT.png)
const static char* test_root_ca PROGMEM = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
    "-----END CERTIFICATE-----\n";



#include <NewPingESP8266.h>
#include <WiFiClientSecure.h>
#if defined(ESP32)
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */
#include <WiFi.h>
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450
    
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define pinTrigger    5 //D1
#define pinEcho       4 //D2
#define maxVzdialenost 450
X509List cert(test_root_ca);
#endif

NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

WiFiClientSecure client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.println(F("Wifi pripojene s IP:"));
  Serial.println(WiFi.localIP());
#if defined(ESP32)
  client.setCACert(test_root_ca);
#elif defined(ESP8266)
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
#endif
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
    client.stop();
    if (client.connect(host, 443)) {
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
    }
  }
  else {
    Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
  }
  client.stop();
#if defined(ESP32)
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  esp_deep_sleep_start();
#elif defined(ESP8266)
  Serial.println(F("Entering deep sleep mode for 300 seconds"));
  ESP.deepSleep(300e6);
#endif
}

void loop() {

}
