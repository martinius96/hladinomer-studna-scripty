/*|--------------------------------------------------------------------------------------|*/
/*|Project: Water Level monitor - Sigfox 868 MHz UART - HC-SR04 / JSN-SR04T / HY-SRF05   |*/
/*|Arduino, ESP8266 (NodeMCU), ESP32 (DevKit)                                            |*/
/*|Autor: Ing. Martin Chlebovec (martinius96)                                            |*/
/*|E-mail: martinius96@gmail.com                                                         |*/
/*|Project info, schematics: https://martinius96.github.io/hladinomer-studna-scripty/en/ |*/
/*|Test web interface for HTTP: http://arduino.clanweb.eu/studna_s_prekladom/            |*/
/*|Test web interface for HTTPS: https://hladinomer.000webhostapp.com/                   |*/
/*|Revízia: 9. February 2022                                                             |*/
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

#include <SoftwareSerial.h>

//SW Uart for Sigfox MODEM WISOL
#define TX 7
#define RX 8
SoftwareSerial Sigfox(RX, TX);

//Library for ultrasonic sensor (use with Arduino)
#include <NewPing.h>
//#include <NewPingESP8266.h> //for ESP8266, ESP32
#define pinTrigger    5
#define pinEcho       6
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
//NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost); // pre ESP8266, ESP32
unsigned long timer = 0;


void setup() {
  Sigfox.begin(9600); //SoftwareSerial
  Serial.begin(115200); //HW SERIAL
}

void loop() {
  if (Sigfox.available()) {
    Serial.write(Sigfox.read());
  }
  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }
  if ((millis() - timer) >= 660000 || timer == 0) { //routine each 11 minutes = 130 messages per day
    timer = millis();
    Sigfox.println(); //Wakeup from Light sleep via ‘\n’ (ASCII 10)
    //Sigfox.print('\n'); //Wakeup from Light sleep via ‘\n’ (ASCII 10) - ekvivalent
    unsigned int vzdialenost = sonar.ping_cm();
    delay(50);
    if (vzdialenost > 0) {
      vzdialenost = 0;
      for (int i = 0; i < 10; i++) {
        vzdialenost += sonar.ping_cm();
        delay(50);
      }
      vzdialenost = vzdialenost / 10;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(F(" cm."));
      char sprava[4]; //4B sprava, max mozna 12B
      unsigned int cislo1 = vzdialenost;
      sprintf(sprava, "%04X", cislo1);
      Serial.print(F("Payload do Sigfox siete: "));
      Serial.print(cislo1);
      Serial.print(F(", hexa tvar: "));
      Serial.println(sprava);
      Sigfox.print(F("AT$SF="));
      Sigfox.println(sprava);
      delay(5000);
      Sigfox.print(F("AT$P=1")); //Light sleep (Send a break (‘\n’) to wake up.)
      //Sigfox.print(F("AT$P=2")); //Deep sleep (power-on reset needed for wake up)
    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      timer = 0; //vynulujeme timer, vykoname nove meranie
    }
  }
}
