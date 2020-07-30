/*|-------------------------------------------------|*/
/*|Projekt: Hladinomer - ESP32 - HTTPS              |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Web: https://arduino.php5.sk                     |*/
/*|Licencia pouzitia: MIT                           |*/
/*|Revízia: 26. Marec 2020                          |*/
/*|-------------------------------------------------|*/

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <NewPingESP8266.h>
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

const char * ssid = "wifi_meno"; //meno wifi siete
const char * password = "wifi_heslo"; //heslo na wifi siet
const char * host = "arduino.php5.sk"; //bez https a bez www

const char* test_root_ca = \
                           "-----BEGIN CERTIFICATE-----\n" \
                           "MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh\n" \
                           "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
                           "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
                           "QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT\n" \
                           "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
                           "b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n" \
                           "AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n\n" \
                           "+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28\n" \
                           "OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK\n" \
                           "MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki\n" \
                           "f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik\n" \
                           "KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw\n" \
                           "HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW\n" \
                           "0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n" \
                           "BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n" \
                           "MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n" \
                           "OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n" \
                           "YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH\n" \
                           "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG\n" \
                           "BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc\n" \
                           "YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ\n" \
                           "/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG\n" \
                           "hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry\n" \
                           "qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ\n" \
                           "ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX\n" \
                           "TVyMnGo=\n" \
                           "-----END CERTIFICATE-----\n";
WiFiClientSecure client;
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  client.setCACert(test_root_ca);
  Serial.println("");
  Serial.println("Wifi pripojene s IP:");
  Serial.println(WiFi.localIP());
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
    String url = "/studna/data.php";
    if (client.connect(host, 443)) {
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
  }
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");
  esp_deep_sleep_start();
}

void loop() {

}
