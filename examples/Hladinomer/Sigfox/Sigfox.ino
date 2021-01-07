/*|---------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - Arduino + Sigfox Modem (868MHz)                |*/
/*|Autor: Martin Chlebovec                                              |*/
/*|E-mail: martinius96@gmail.com                                        |*/
/*|Licencia pouzitia: MIT                                               |*/
/*|Revízia: 7. Január 2021                                              |*/
/*|---------------------------------------------------------------------|*/

/*|---------------------------------------------------------------------|*/
/*|Inštrukcie pre nastavenie Sigfox Modemu na stránkach LPWAN:          |*/
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
#define pinTrigger    5
#define pinEcho       6
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
unsigned long timer = 0;


void setup() {
  Sigfox.begin(9600);
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
  if ((millis() - timer) >= 660000 || timer == 0) { //rutina raz za 11 minut (limit 140 sprav za den), odosle sa 130 správ za deň
    timer = millis();
    int vzdialenost = sonar.ping_cm();
    delay(50);
    if (vzdialenost > 0) {
      vzdialenost = 0;
      for (int i = 0; i < 5; i++) {
        vzdialenost += sonar.ping_cm();
        delay(50);
      }
      wdt_reset();
      vzdialenost = vzdialenost / 5;
      Serial.print(F("Vzdialenost medzi senzorom a predmetom je: "));
      Serial.print(vzdialenost);
      Serial.println(" cm.");
      char sprava[4]; //4B sprava, max mozna 12B
      unsigned int cislo1 = vzdialenost;
      sprintf(sprava, "%04X", cislo1);
      Serial.print("Do Sigfox siete odosielam tento payload: ");
      Serial.print(cislo1);
      Serial.print(", hexa tvar: ");
      Serial.println(sprava);
      wdt_reset();
      Sigfox.print("AT$SF=");
      Sigfox.println(sprava);
      wdt_reset();
      delay(1000);
    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
    }
  }
}
