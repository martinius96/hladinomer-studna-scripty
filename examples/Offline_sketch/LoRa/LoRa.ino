//Water level monitor - ESP32 + LoRa (PPP, no LoRaWAN)
//LoRa node for PPP communication (Semtech SX1276/77/78/79)
//Adjustable frequency 868, 915 or 433 MHz
//Receiver must parse packet (e.g. using struct) and then execute the HTTP(S) request to Watmonitor webapp
//Created by: Martin Chlebovec (Your-IoT)

#include <LoRa.h>
#include <NewPingESP8266.h> //for ESP8266, ESP32

//Ultrasonic pinout
#define pinTrigger    7
#define pinEcho       6

//Semtech pinout
#define SS 5
#define RST 14
#define DI0 2

#define maxdistance 450
NewPingESP8266 sonar(pinTrigger, pinEcho, maxdistance); // for ESP8266, ESP32
unsigned long timer = 0;

//Macros with frequency bands (433 MHz possible also in Europe)
#define EUROPE 868E6
#define USA 915E6
#define ASIA 433E6

//Data structure
struct DataPacket {
  unsigned int distance;
};

void setup() {
  Serial.begin(115200);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(EUROPE)) { //or use different county setting
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa started");
  LoRa.setTxPower(8); //set TX power to 8 dB
  LoRa.setSpreadingFactor(9); //SF9
  LoRa.setSignalBandwidth(125E3); //Bandwidth 125 kHz
  LoRa.setCodingRate4(8); //Coding rate 4/8 (4 coding bits for each 4 bits)
  LoRa.setPreambleLength(8); //for better synchronization use preamble
  LoRa.setSyncWord(0x12); //Syncword for better packet filtering
  LoRa.enableCrc(); //Turn on CRC
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
      DataPacket packet;
      packet.distance = distance;
      Serial.print("Distance: "); Serial.print(packet.distance);
      Serial.println(" cm");
      Serial.println("Sending data");
      LoRa.beginPacket();
      LoRa.write((uint8_t*)&packet, sizeof(packet));
      LoRa.endPacket();
      Serial.println("Packet sent");
    } else {
      Serial.println(F("Distance between water level and sensor is OUT of range, new measurement will be made"));
      timer = 0; //do new measurement
    }
    LoRa.sleep(); //power-saving mode when not sending
  }
}
