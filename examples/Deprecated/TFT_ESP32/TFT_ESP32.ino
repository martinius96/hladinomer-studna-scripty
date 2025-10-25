/*|-----------------------------------------------------------------------------------|*/
/*|Project: Water level monitor - HTTPS - FreeRTOS - HC-SR04 / JSN-SR04T / HY-SRF05   |*/
/*|ESP32 (DevKit, Generic)                                                            |*/
/*|Autor: Martin Chlebovec (martinius96)                                              |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Project info: https://martinius96.github.io/hladinomer-studna-scripty/en/          |*/
/*|Test web interface: https://arduino.clanweb.eu/studna_s_prekladom/?lang=en         |*/
/*|Buy me coffee: paypal.me/chlebovec                                                 |*/
/*|Revision: 9. Nov 2023                                                              |*/
/*|-----------------------------------------------------------------------------------|*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <NewPingESP8266.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

const char* host = "hladinomer.eu"; //webhost
String url = "/data.php"; //URL address to PHP file
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

//Root CA cert - ISRG Root X1
const static char* test_root_ca PROGMEM = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
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
