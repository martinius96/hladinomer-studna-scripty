/*|-----------------------------------------------------------------------------------|*/
/*|Project: Water level monitor - HTTPS - FreeRTOS - HC-SR04 / JSN-SR04T / HY-SRF05   |*/
/*|ESP32 (DevKit, Generic)                                                            |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Project info: https://martinius96.github.io/hladinomer-studna-scripty/en/          |*/
/*|Test web interface: https://waterwell-arduino.000webhostapp.com/?lang=en           |*/
/*|Buy me coffee: paypal.me/chlebovec                                                 |*/
/*|Revision: 1. January 2022                                                          |*/
/*|-----------------------------------------------------------------------------------|*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <NewPingESP8266.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
const char* host = "waterwell-arduino.000webhostapp.com"; //webserver address (domain)
String url = "/data.php"; //PHP file under domain host for storing data into MySQL
String url2 = "/json_output.php"; //PHP file with JSON output under domain host

#define pinTrigger    22
#define pinEcho       21 //Changed from D23, it is used for SPI communication with TFT display
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);
TFT_eSPI tft = TFT_eSPI();
TaskHandle_t Task1; //ULTRASONIC MEASUREMENT
TaskHandle_t Task2; //WIFI HTTP SOCKET
QueueHandle_t  q = NULL;

WiFiClientSecure client;
WiFiClientSecure client2;
WiFiManager wm;
int success_flag = 0;
static void Task1code( void * parameter);
static void Task2code( void * parameter);

//DigiCert Global Root CA in .pem format, stored in PROGMEM flash
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

void setup() {
  Serial.begin(115200);
  WiFi.begin();
  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH); //100% brightness
  //analogWrite(27, 127); //50% brightness for saving battery, comment digitalWrite above
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.setTextSize(2);
  uint16_t calData[5] = { 397, 3463, 297, 3548, 3 };
  tft.setTouch(calData);
  if (WiFi.status() != WL_CONNECTED) {
    tft.fillScreen(TFT_OLIVE);
    tft.fillRoundRect(20, 7, 280, 220, 21, TFT_BLACK);
    tft.drawRoundRect(30, 15, 260, 40, 10, TFT_ORANGE);
    tft.setTextColor(TFT_RED);
    tft.setCursor(42, 30);
    tft.print(" WIFI CONFIGURATION ");
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(30, 70);
    tft.print("1. Connect to WiFi");
    tft.setCursor(80, 100);
    tft.setTextColor(TFT_CYAN);
    tft.print("Ultrasonic_node");
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(30, 130);
    tft.print("2. Set your WiFi");
    tft.setCursor(30, 160);
    tft.print("3. Add its password");
    tft.setCursor(30, 190);
    tft.print("4. Enjoy sensor node");
  }
  wm.autoConnect("Ultrasonic_node");
  tft.fillScreen(TFT_GREENYELLOW);
  tft.fillRoundRect(20, 7, 280, 220, 21, TFT_BLACK);
  tft.drawRoundRect(30, 15, 260, 40, 10, TFT_ORANGE);
  tft.setTextColor(TFT_RED);
  tft.setCursor(42, 30);
  tft.print("   WIFI CONNECTED   ");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(30, 70);
  tft.print("DHCP IP address:");
  tft.setCursor(80, 100);
  tft.setTextColor(TFT_CYAN);
  tft.print(WiFi.localIP());
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(30, 130);
  tft.print("Measuring...");
  tft.setCursor(30, 160);
  tft.print("Reading JSON payload");
  Serial.println(F("Wifi connected with IP:"));
  Serial.println(WiFi.localIP());
  client.setCACert(test_root_ca);
  client2.setCACert(test_root_ca);
  q = xQueueCreate(20, sizeof(int));
  if (q != NULL) {
    Serial.println(F("Queue FIFO buffer is created"));
    vTaskDelay(1000 / portTICK_PERIOD_MS); //wait for a second
    xTaskCreatePinnedToCore(
      Task1code,   /* Task function. */
      "Task1",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &Task1,      /* Task handle to keep track of created task */
      1);          /* pin task to core 1 */
    Serial.println(F("Ultrasonic measurement task started"));
    xTaskCreatePinnedToCore(
      Task2code,   /* Task function. */
      "Task2",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &Task2,      /* Task handle to keep track of created task */
      0);          /* pin task to core 0 */
    Serial.println(F("HTTP Socket task started"));
  } else {
    Serial.println(F("Queue creation failed"));
  }
}

void loop() {
  yield();
}

static void Task1code( void * parameter) {
  if (q == NULL) {
    Serial.println(F("Queue in Measurement task is not ready"));
    return;
  }
  while (1) {
    int distance = sonar.ping_cm();
    delay(50);
    Serial.print(F("Test measurement: "));
    Serial.print(distance);
    Serial.println(F(" cm"));
    if (distance > 0) {
      distance = 0;
      for (int i = 0; i < 10; i++) {
        distance += sonar.ping_cm();
        delay(50);
      }
      distance = distance / 10;
      Serial.print(F("Distance to water level is: "));
      Serial.print(distance);
      Serial.println(F(" cm."));
      xQueueSend(q, (void *)&distance, (TickType_t )0); //add the measurement value to Queue
      for (int countdown = 300; countdown >= 0; countdown--) {
        Serial.print(F("Next measurement in: "));
        Serial.print(countdown);
        Serial.println(F(" seconds"));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
    }
  }
}
static void Task2code( void * parameter) {
  int distance;
  if (q == NULL) {
    Serial.println(F("Queue in HTTP socket task is not ready"));
    return;
  }
  while (1) {
    xQueuePeek(q, &distance, portMAX_DELAY); //read measurement value from Queue and run code below, if no value, WAIT....
    String data = "hodnota=" + String(distance) + "&token=123456789";
    //FIRST REQUEST - POST REQUEST WITH DATA SEND
    client.stop();
    if (client.connect(host, 443)) {
      Serial.println(F("Connected to server successfully"));
      client.println("POST " + url + " HTTP/1.0");
      client.println("Host: " + (String)host);
      client.println(F("User-Agent: ESP"));
      client.println(F("Connection: close"));
      client.println(F("Content-Type: application/x-www-form-urlencoded;"));
      client.print(F("Content-Length: "));
      client.println(data.length());
      client.println();
      client.println(data);
      Serial.println(F("Datas were sent to server successfully"));
      success_flag++;
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }
      String line = client.readStringUntil('\n');
    } else {
      Serial.println(F("Connection to webserver was NOT successful"));
      success_flag = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); //wait for a second
    //SECOND REQUEST - GET AND JSON PROCESSING
    client2.stop();
    if (client2.connect(host, 443)) {
      Serial.println(F("Connected to server successfully"));
      client2.print(String("GET ") + url2 + " HTTP/1.0\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP\r\n" + "Connection: close\r\n\r\n");
      Serial.println(F("Datas were sent to server successfully"));
      while (client2.connected()) {
        String line = client2.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(128);
      String line = client2.readString();
      Serial.println(line);
      deserializeJson(doc, line);
      JsonObject obj = doc.as<JsonObject>();
      int vyska = obj[String("value")];
      float objem = obj[String("volume")];
      if (vyska > 0) { //If JSON is not parsed successfully, it will print 0 (that normally never happens as minimum measurement is 2 cm)
        Serial.print("Vyska hladiny: ");
        Serial.print(vyska);
        Serial.println(" cm");
        Serial.print("Objem studne: ");
        Serial.print(objem);
        Serial.println(" litrov");
        tft.fillScreen(TFT_DARKCYAN);
        tft.fillRoundRect(20, 7, 280, 220, 21, TFT_BLACK);
        tft.drawRoundRect(30, 15, 260, 40, 10, TFT_ORANGE);
        tft.setTextColor(TFT_RED);
        tft.setCursor(42, 30);
        tft.print("     WATER LEVEL    ");
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(30, 70);
        tft.print("Height: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(vyska);
        tft.print(" cm");
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(30, 130);
        tft.print("Volume: ");
        tft.setTextColor(TFT_CYAN);
        tft.print(objem);
        tft.print(" L");
        tft.setTextColor(TFT_WHITE);
        success_flag++;
      }
      if (success_flag == 2) { //both requests successful, JSON payload parsed successfully
        xQueueReset(q); //EMPTY QUEUE, IF REQUEST WAS SUCCESSFUL, OTHERWISE RUN REQUEST AGAIN
        Serial.println("Queue is empty");
      }
      success_flag = 0;
    } else {
      Serial.println(F("Connection to webserver was NOT successful"));
      success_flag = 0;
    }
  }
}
