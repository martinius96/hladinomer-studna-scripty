/*|----------------------------------------------------------------------------|*/
/*|Project: Ultrasonic water level monitor - HC-SR04 / JSN-SR04T / HY-SRF05... |*/
/*|Arduino / ESP32 / ESP8266 + GSM SIM800 GPRS/GSM module                      |*/
/*|Author: Martin Chlebovec (martinius96)                                      |*/
/*|E-mail: martinius96@gmail.com                                               |*/
/*|Buy me a coffee at: paypal.me/chlebovec                                     |*/
/*|Project info: https://martinius96.github.io/hladinomer-studna-scripty/en    |*/
/*|Test web interface: http://arduino.clanweb.eu/studna_s_prekladom/?lang=en   |*/
/*|Revision: 17. September 2022                                                |*/
/*|----------------------------------------------------------------------------|*/

#include <SoftwareSerial.h>
#include <NewPing.h> //Arduino (ATmega) compatible
//#include <NewPingESP8266.h> //ESP32 or ESP32 compatible

//SIM800 RELATED - SW UART
#define TX 11
#define RX 10
SoftwareSerial SIM800(RX, TX);

unsigned long timer = 0;

//WEBHOSTING RELATED
const char* host = "arduino.clanweb.eu";
const char* url = "/studna_s_prekladom/data.php";

//ULTRASONIC RELATED
#define pinTrigger    4 //CONNECT TO TRIGGER PIN OF ULTRASONIC SENSOR
#define pinEcho       5 //CONNECT TO ECHO PIN OF ULTRASONIC SENSOR
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost); //Arduino (ATmega) compatible
//NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost); //ESP32 or ESP32 compatible

void ShowSerialData()
{
  while (SIM800.available() != 0)
    Serial.write(SIM800.read());
  delay(1000);
}

void setup() {
  SIM800.begin(9600); //SoftwareSerial
  Serial.begin(9600); //HW SERIAL
}

void loop() {
  if ((millis() - timer) >= 300000 || timer == 0) { //rutina kazdych 300 = 5 min
    timer = millis();
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
      String payload = "hodnota=" + String(distance) + "&token=123456789"; //PAYLOAD
      SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");//APN
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+SAPBR=1,1");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+SAPBR=2,1");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPINIT");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPPARA=\"CID\",1");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPPARA=\"URL\",\"http://" + String(host) + String(url) + "\"");
      delay(4000);
      ShowSerialData();
      SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"");
      delay(4000);
      ShowSerialData();
      SIM800.println("AT+HTTPDATA=" + String(payload.length()) + ",100000");
      delay(6000);
      ShowSerialData();
      SIM800.println(payload);
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPACTION=1");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPREAD");
      delay(6000);
      ShowSerialData();
      SIM800.println("AT+HTTPTERM");
      delay(10000);
      ShowSerialData();
    } else {
      timer = 0;
    }
  }
}
