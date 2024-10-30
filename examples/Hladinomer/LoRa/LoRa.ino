//Water level monitor - ESP32 / ESP8266 / Arduino
//LoRa node for P2P communication RFM9X (Semtech SX127X)
//Adjustable frequency 868, 915 or 433 MHz
//Receiver must parse packet and do request to the IoT platform via HTTP/HTTPS POST request

#include <LoRa.h>
#include <NewPing.h>
//#include <NewPingESP8266.h> // for ESP8266, ESP32
#define pinTrigger    5
#define pinEcho       6
#define maxdistance 450
NewPing sonar(pinTrigger, pinEcho, maxdistance);
//NewPingESP8266 sonar(pinTrigger, pinEcho, maxdistance); // for ESP8266, ESP32
unsigned long timer = 0;
#define EUROPE 868E6
#define USA 915E6
#define ASIA 433E6
void setup() {
  if (!LoRa.begin(EUROPE)) { //or use different county setting
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  if ((millis() - timer) >= 600000 || timer == 0) { //one LoRa send per 10 mins
    timer = millis();
    unsigned int distance = sonar.ping_cm();
    delay(50);
    if (distance > 0) {
      distance = 0;
      for (int i = 0; i < 10; i++) {
        distance += sonar.ping_cm();
        delay(50);
      }
      distance = distance / 10;
      Serial.print(F("Distance between water level and sensor is: "));
      Serial.print(distance);
      Serial.println(F(" cm."));
      Serial.println(F("Sending..."));
      LoRa.beginPacket();
      LoRa.print(distance);
      LoRa.endPacket();
      Serial.println(F("Sent"));
    } else {
      Serial.println(F("Distance between water level and sensor is OUT of range, new measurement will be made"));
      timer = 0; //do new measurement
    }
  }
}
