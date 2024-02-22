/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - ULP - DEEP SLEEP - HC-SR04 / JSN-SR04T / HY-SRF05     |*/
/*|ESP32 (DevKit, Generic)                                                            |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie: http://arduino.clanweb.eu/studna_s_prekladom/         |*/
/*|Knižnica ESP8266NewPing je dostupná v Github repozitári:                           |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť     |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/    |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                               |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv    |*/
/*|Licencia pouzitia: MIT                                                             |*/
/*|Revízia: 22. Február 2024                                                          |*/
/*|-----------------------------------------------------------------------------------|*/
const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis
//Pre testovacie webove rozhranie sa data odosielaju na: arduino.clanweb.eu/studna_s_prekladom/data.php (HTTP POST ONLY)

#include <NewPingESP8266.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450

NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

WiFiClient client;
void setup() {
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(60);
  wifiManager.autoConnect("Hladinomer_AP"); //SSID --> no password
    if (WiFi.status() != WL_CONNECTED) {
    esp_sleep_enable_timer_wakeup(1 * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
    delay(1000);
  }else{
  Serial.println(F(""));
  Serial.println(F("Wifi pripojene s IP:"));
  Serial.println(WiFi.localIP());
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
        while (client.connected()) {
          String line = client.readStringUntil('\n');
          if (line == "\r") {
            break;
          }
        }
        String line = client.readStringUntil('\n');
    } else {
      Serial.println(F("Pripojenie zlyhalo..."));
    }
  }
  else {
    Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
  }
  client.stop();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  delay(100);
  esp_deep_sleep_start();
     }
}

void loop() {

}
