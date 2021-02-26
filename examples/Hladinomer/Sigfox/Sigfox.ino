/*|-----------------------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - HTTP - Sigfox 868 MHz UART - HC-SR04 / JSN-SR04T / HY-SRF05  |*/
/*|Arduino, ESP8266 (NodeMCU), ESP32 (DevKit)                                         |*/
/*|Autor: Bc. Martin Chlebovec (martinius96)                                          |*/
/*|E-mail: martinius96@gmail.com                                                      |*/
/*|Info k projektu (schéma): https://martinius96.github.io/hladinomer-studna-scripty/ |*/
/*|Testovacie webove rozhranie: http://arduino.clanweb.eu/studna_s_prekladom/         |*/
/*|Knižnice NewPing, ESP8266NewPing sú dostupné v Github repozitári:                  |*/
/*|https://github.com/martinius96/hladinomer-studna-scripty/ - stihnuť a rozbaliť     |*/
/*|Obsah priečinka /src/ nakopírovať do C:/Users/User/Dokumenty/Arduino/libraries/    |*/
/*|Na toto webove rozhranie posiela mikrokontroler data                               |*/
/*|Na zaklade zvolenej platformy v Arduino IDE sa vykona kompilacia podla direktiv    |*/
/*|Licencia pouzitia: MIT                                                             |*/
/*|Revízia: 26. Februar 2021                                                          |*/
/*|-----------------------------------------------------------------------------------|*/

/*|---------------------------------------------------------------------|*/
/*|Inštrukcie pre nastavenie Sigfox Modemu na stránkach Sigfox backend: |*/
/*|Callbacks --> NEW --> Custom callback                                |*/
/*|UPLINK, TYPE: DATA, CHANNEL: URL                                     |*/
/*|Do Custom payload config napíšeme: cislo1::uint:16                   |*/
/*|Do URL pattern: http://arduino.clanweb.eu/studna_s_prekladom/data.php|*/
/*|PRIPADNE: (pripadne vas URL webservera s možnostou TLS - SSL)        |*/
/*|V HTTP metóde zvolíme: POST (jediná podporovaná metóda Hladinomeru)  |*/
/*|Do Body (tela správy) doplníme:                                      |*/
/*|hodnota={customData#cislo1}                                          |*/
/*|Mozno odosielat aj systemove udaje - cislo spravy, RSSI, GEO UDAJE   |*/
/*|Do Content-Type: application/x-www-form-urlencoded                   |*/
/*|---------------------------------------------------------------------|*/

//HLAVICKOVE SUBORY watchdog a software serial
#include <avr\wdt.h>
#include <SoftwareSerial.h>

// nastavenie softverovej zbernice pre Sigfox Modem
#define TX 7
#define RX 8
SoftwareSerial Sigfox(RX, TX);

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
  Sigfox.begin(9600); //SoftwareSerial
  Serial.begin(115200);
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  if (Sigfox.available()) {
    Serial.write(Sigfox.read()); //MOZNO VYSKUSAT AT prikaz, odpoved OK
  }
  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }
  if ((millis() - timer) >= 660000 || timer == 0) { //rutina raz za 11 minut (limit 140 sprav za den), odosle sa 130 správ za deň (24h)
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
      wdt_reset();
      vzdialenost = vzdialenost / 10;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(F(" cm."));
      char sprava[4]; //4B sprava, max mozna 12B
      unsigned int cislo1 = vzdialenost;
      sprintf(sprava, "%04X", cislo1);
      Serial.print(F("Do Sigfox siete odosielam tento payload: "));
      Serial.print(cislo1);
      Serial.print(F(", hexa tvar: "));
      Serial.println(sprava);
      wdt_reset();
      Sigfox.print(F("AT$SF="));
      Sigfox.println(sprava);
      wdt_reset();
      delay(1000);
      Sigfox.print(F("AT$P=1")); //Light sleep (Send a break (‘\n’) to wake up.)
      //Sigfox.print(F("AT$P=2")); //Deep sleep (power-on reset needed for wake up)
    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
      timer = 0; //vynulujeme timer, vykoname nove meranie
    }
  }
}
