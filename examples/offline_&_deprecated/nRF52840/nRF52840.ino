//nRF52840 (also Sense version compatible)
//Measurement of water level height (distance) using proximity ultrasonic sensor HC-SR04

#include <Adafruit_TinyUSB.h>
const int trigPin = 5;  // Pin pre Trig
const int echoPin = 4; // Pin pre Echo
int distance = 0;
int i = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  while (1) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    distance += (duration * 0.0343) / 2;
    i++;
    if (i == 4) {
      break;
    }
  }
  i = 0;
  distance = distance / 5;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  distance = 0;
  delay(2000);
}
