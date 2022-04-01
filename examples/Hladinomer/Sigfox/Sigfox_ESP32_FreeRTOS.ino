/*|--------------------------------------------------------------------------------------|*/
/*|Project: Water Level monitor - Sigfox 868 MHz (RC1) - HC-SR04 / JSN-SR04T / HY-SRF05  |*/
/*|ESP32 (DevKit) + FreeRTOS, Core: 2.0.2 release                                        |*/
/*|Author: Ing. Martin Chlebovec (martinius96)                                           |*/
/*|E-mail: martinius96@gmail.com                                                         |*/
/*|Project info, schematics: https://martinius96.github.io/hladinomer-studna-scripty/en/ |*/
/*|Test web interface for HTTP: http://arduino.clanweb.eu/studna_s_prekladom/            |*/
/*|Test web interface for HTTPS: https://hladinomer.000webhostapp.com/                   |*/
/*|Revision: 1. April 2022                                                               |*/
/*|--------------------------------------------------------------------------------------|*/

/*|---------------------------------------------------------------------------------|*/
/*|Instructions to set Sigfox backend:                                              |*/
/*|Callbacks --> NEW --> Custom callback                                            |*/
/*|UPLINK, TYPE: DATA, CHANNEL: URL                                                 |*/
/*|To Custom payload config write: cislo1::uint:16                                  |*/
/*|To URL pattern add (HTTP): http://arduino.clanweb.eu/studna_s_prekladom/data.php |*/
/*|IF WANT TO USE HTTPS (SSL) CONENCTION WRITE BELOW:                               |*/
/*|To URL pattern add (HTTPS): https://hladinomer.000webhostapp.com/data.php        |*/
/*|In HTTP method use: POST                                                         |*/
/*|To Body of message add: hodnota={customData#cislo1}&token=123456789              |*/
/*|At Content-Type set: application/x-www-form-urlencoded                           |*/
/*|---------------------------------------------------------------------------------|*/

#include <NewPingESP8266.h>


//HW UART for Sigfox MODEM WISOL WSSFM10R1
#define TX 17
#define RX 16
HardwareSerial Sigfox(2);

//Ultrasonic sensor related
#define pinTrigger    22
#define pinEcho       23
#define maxVzdialenost 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

//Task and Queue handlers
TaskHandle_t Task1; //ULTRASONIC MEASUREMENT
TaskHandle_t Task2; //Sigfox communication task
QueueHandle_t  q = NULL;

//declarations of Task related functions
static void Task1code( void * parameter);
static void Task2code( void * parameter);

void setup() {
  Sigfox.begin(115200, SERIAL_8N1, RX TX;
  Serial.begin(115200);
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
    Serial.println(F("Sigfox communication task started"));
  } else {
    Serial.println(F("Queue creation failed"));
  }
}

void loop() {
  if (Sigfox.available()) {
    Serial.write(Sigfox.read());
  }
  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }
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
    Serial.println(F("Queue in Sigfox communication task is not ready"));
    return;
  }
  while (1) {
    xQueueReceive(q, &distance, portMAX_DELAY); //read measurement value from Queue and run code below, if no value, WAIT....
    Sigfox.println(); //Wakeup from Light sleep via ‘\n’ (ASCII 10)
    //Sigfox.print('\n'); //Wakeup from Light sleep via ‘\n’ (ASCII 10) - ekvivalent
    char payload_message[4]; //4B message, maximum 12B can be used total
    unsigned int cislo1 = distance;
    sprintf(payload_message, "%04X", cislo1);
    Serial.print(F("Payload sent to Sigfox backend: "));
    Serial.print(cislo1);
    Serial.print(F(", HEX: "));
    Serial.println(payload_message);
    Sigfox.print(F("AT$SF="));
    Sigfox.println(payload_message);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Sigfox.print(F("AT$P=1")); //Light sleep (Send a break (‘\n’) to wake up.)
    //Sigfox.print(F("AT$P=2")); //Deep sleep (power-on reset needed for wake up)
  }
}
