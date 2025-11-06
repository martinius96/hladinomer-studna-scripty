
/*|-------------------------------------------------------------------------------|*/
/*|Project: Water level monitor - HTTP - ULP - ESP32                              |*/
/*|ToF laser ranging sensor 0-400 cm - VL53L1X (TOF400C)                          |*/
/*|Author: Martin Chlebovec Your-IoT (martinius96)                                |*/
/*|E-mail: martinius96@gmail.com                                                  |*/
/*|Info, schematics: https://martinius96.github.io/hladinomer-studna-scripty/en/  |*/
/*|Test web interface: http://arduino.clanweb.eu/studna_s_prekladom/?lang=en      |*/
/*|Revision: 6th Nov 2025                                                         |*/
/*|-------------------------------------------------------------------------------|*/

#include <WiFi.h>
#include <Wire.h>
#include <VL53L1X.h>

const char * ssid = "SSID_NAME"; //WiFi SSID
const char * password = "WIFI_PASSWORD"; //WiFi PASSWORD

//HOST
const char* host = "arduino.clanweb.eu"; //host (server)
String url = "/data.php"; //URL address to to target PHP file

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  300
RTC_DATA_ATTR unsigned long BootCount;
VL53L1X sensor;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  Wire.begin(22, 23);
  Wire.setClock(400000); // use 400 kHz I2C
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }

  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000); // 50 ms
  sensor.startContinuous(50); // Meranie každých 50 ms
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println(F(""));
  Serial.println(F("WiFi connected, IP:"));
  Serial.println(WiFi.localIP());
  int vzdialenost =  sensor.read() / 10;
  delay(50);
  if (vzdialenost > 0) {
    vzdialenost = 0;
    for (int i = 0; i < 10; i++) {
      vzdialenost += sensor.read() / 10;
      delay(50);
    }
    sensor.stopContinuous();
    vzdialenost = vzdialenost / 10;
    Serial.print(F("Distance is: "));
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
      Serial.println(F("Data successfully sent"));
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }
      String line = client.readStringUntil('\n');
    } else {
      Serial.println(F("Connection wasnt performed..."));
    }
  }
  else {
    Serial.println(F("Distance between laser sensor and surface is out of range."));
  }
  client.stop();
  delay(250);
  if (BootCount < 1) {
    delay(30000);
  }
  BootCount++;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  esp_deep_sleep_start();
}

void loop() {

}
