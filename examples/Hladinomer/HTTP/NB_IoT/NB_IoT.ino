/*|------------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - SIM7000 - NB-IoT - HC-SR04 / JSN-SR04T / HY-SRF05      |*/
/*|Arduino, ESP8266 (NodeMCU), ESP32 (DevKit)                                          |*/
/*|Autor: Ing. Martin Chlebovec (martinius96)                                          |*/
/*|E-mail: martinius96@gmail.com                                                       |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/  |*/
/*|Testovacie webove rozhranie pre HTTP: http://arduino.clanweb.eu/studna_s_prekladom/ |*/
/*|Testovacie webove rozhranie pre HTTPS: https://hladinomer.000webhostapp.com/        |*/
/*|Knižnice NewPing, ESP8266NewPing sú dostupné v Github repozitári:                   |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť      |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/     |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                                |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv     |*/
/*|Licencia pouzitia: MIT                                                              |*/
/*|Revízia: 13. Januar 2022                                                            |*/
/*|------------------------------------------------------------------------------------|*/

#include <DFRobot_SIM7000.h>

#define PIN_TX     7
#define PIN_RX     8
#include <SoftwareSerial.h>

//This URL is use for post data to webapp
#define POSTURL   "arduino.clanweb.eu/studna_s_prekladom/data.php"

// nastavenie softverovej zbernice pre SIM7000E modul
SoftwareSerial     mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7000    sim7000;
//PREMENNE, HLAVICKOVY SUBOR, OBJEKT PRE HC-SR04 / JSN-SR04T
#include <NewPing.h>
//#include <NewPingESP8266.h> // pre ESP8266, ESP32
#define pinTrigger    5
#define pinEcho       6
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
//NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost); // pre ESP8266, ESP32
unsigned long timer = 0;


void setup() {
  int signalStrength, dataNum;
  Serial.begin(115200);
  while (!Serial);
  sim7000.begin(mySerial);
  Serial.println("Turn ON SIM7000......");
  if (sim7000.turnON()) {                                  //Turn ON SIM7000
    Serial.println("Turn ON !");
  }

  Serial.println("Set baud rate......");
  while (1) {
    if (sim7000.setBaudRate(19200)) {                    //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
      Serial.println("Set baud rate:19200");
      break;
    } else {
      Serial.println("Faile to set baud rate");
      delay(1000);
    }
  }
  Serial.println("Check SIM card......");
  if (sim7000.checkSIMStatus()) {                          //Check SIM card
    Serial.println("SIM card READY");
  } else {
    Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7000");
    while (1);
  }

  Serial.println("Set net mode......");
  while (1) {
    if (sim7000.setNetMode(GPRS)) {                      //Set net mod GPRS
      Serial.println("Set GPRS mode");
      break;
    } else {
      Serial.println("Fail to set mode");
      delay(1000);
    }
  }

  Serial.println("Get signal quality......");
  signalStrength = sim7000.checkSignalQuality();           //Check signal quality from (0-30)
  Serial.print("signalStrength =");
  Serial.println(signalStrength);
  delay(500);
  Serial.println("Attaching service......");
  while (1) {
    if (sim7000.attacthService()) {                      //Open the connection
      Serial.println("Attach service");
      break;
    } else {
      Serial.println("Fail to Attach service");
      delay(1000);
    }
  }

  Serial.println("Init http......");
  while (1) {
    if (sim7000.httpInit(GPRS)) {                        //Init http service
      Serial.println("HTTP init !");
      break;
    } else {
      Serial.println("Fail to init http");
    }
  }
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  if ((millis() - timer) >= 660000 || timer == 0) { //rutina raz za 11 minut (limit 140 sprav za den), odosle sa 130 správ za deň (24h)
    timer = millis();
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
      wdt_reset();
      String data = "hodnota=" + String(vzdialenost) + "&token=123456789";                                    //}
      while (1) {
        if (sim7000.httpPost(POSTURL, data)) {           //HTTP POST
          Serial.println("Post successed");
          break;
        } else {
          Serial.println("Fail to post");
        }
      }

    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      timer = 0; //vynulujeme timer, vykoname nove meranie
    }
    /*
      Serial.println("Disconnect");
      sim7000.httpDisconnect();                                //Disconnect
      Serial.println("Close net work");
      sim7000.closeNetwork();                                  //Close net work
      Serial.println("Turn off SIM7000");
      sim7000.turnOFF();                                       //Turn OFF SIM7000
    */
  }
}
