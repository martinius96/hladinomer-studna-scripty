//Water level monitor - ESP32 / ESP8266 / Arduino
//LoRa node for P2P communication
//Adjustable frequency 868, 915 or 433 MHz
//Receiver must do request to platform

#include <LoRa.h>
#include <NewPing.h>
//#include <NewPingESP8266.h> // pre ESP8266, ESP32
#define pinTrigger    5
#define pinEcho       6
#define maxdistance 450
NewPing sonar(pinTrigger, pinEcho, maxdistance);
//NewPingESP8266 sonar(pinTrigger, pinEcho, maxdistance); // pre ESP8266, ESP32
unsigned long timer = 0;

void setup() {
  if (!LoRa.begin(868E6)) { //433E6, 915E6
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
      LoRa.beginPacket();
      LoRa.print(distance);
      LoRa.endPacket();
    } else {
      Serial.println(F("Distance between water level and sensor is OUT of range, new measurement will be made"));
      timer = 0; //vynulujeme timer, vykoname nove meranie
    }
  }
}
