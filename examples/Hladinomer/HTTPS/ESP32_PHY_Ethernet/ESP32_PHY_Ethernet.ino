/*|----------------------------------------------------------------------------|*/
/*|Project: Ultrasonic water level monitor - HC-SR04 / JSN-SR04T / HY-SRF05... |*/
/*|ESP32 (DevKit, Generic) + PHY Ethernet LAN8720 / TLK110, RMII interface     |*/
/*|Author: Martin Chlebovec (martinius96)                                      |*/
/*|E-mail: martinius96@gmail.com                                               |*/
/*|Buy me a coffee at: paypal.me/chlebovec                                     |*/
/*|Project info: https://martinius96.github.io/hladinomer-studna-scripty/en    |*/
/*|Test web interface: https://hladinomer.eu/?lang=en                          |*/
/*|Revision: 31. Oct. 2024                                                     |*/
/*|----------------------------------------------------------------------------|*/

#include <WiFiClientSecure.h>
#include <ETH.h>
#include <NewPingESP8266.h>

const char* host = "hladinomer.eu"; //host (server)
String url = "/data.php"; //URL address to to target PHP file

boolean eth_state = false;
#define pinTrigger    4
#define pinEcho       5
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

#ifdef ETH_CLK_MODE
#undef ETH_CLK_MODE
#endif
#define ETH_CLK_MODE    ETH_CLOCK_GPIO17_OUT
#define ETH_POWER_PIN   -1
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_ADDR        1
#define ETH_MDC_PIN     23
#define ETH_MDIO_PIN    18

WiFiClientSecure client; //Secured client object for HTTPS connection
TaskHandle_t Task1; //ULTRASONIC MEASUREMENT
TaskHandle_t Task2; //PHY ETHERNET HTTP SOCKET
QueueHandle_t  q = NULL;

//Root CA cert - ISRG Root X1
const static char* test_root_ca PROGMEM = \
    "-----BEGIN CERTIFICATE-----\n" \
    "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\n" \
    "-----END CERTIFICATE-----\n";


static void Task1code( void * parameter);
static void Task2code( void * parameter);
void WiFiEvent(WiFiEvent_t event);

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
  delay(5000);
  client.setCACert(test_root_ca);
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
    Serial.println(F("HTTPS Socket task started"));
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
    client.stop();
    while (eth_state != true) {
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
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
      xQueueReset(q); //EMPTY QUEUE, IF REQUEST WAS SUCCESSFUL, OTHERWISE RUN REQUEST AGAIN
      while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }
      String line = client.readStringUntil('\n');
    } else {
      Serial.println(F("Connection to webserver was NOT successful"));
    }
    yield();
    client.stop();
  }
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      eth_state = false;
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      eth_state = false;
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      eth_state = true;
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      eth_state = false;
      Serial.println("ETH Disconnected");
      break;
    case ARDUINO_EVENT_ETH_STOP:
      eth_state = false;
      Serial.println("ETH Stopped");
      break;
    default:
      break;
  }
}
