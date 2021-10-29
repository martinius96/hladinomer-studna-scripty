/*|---------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - Zrážkomer - Arduino + Sigfox Modem (868MHz)    |*/
/*|Autor: Martin Chlebovec                                              |*/
/*|E-mail: martinius96@gmail.com                                        |*/
/*|Licencia pouzitia: MIT                                               |*/
/*|Revízia: 7. Január 2021                                              |*/
/*|---------------------------------------------------------------------|*/

/*|---------------------------------------------------------------------|*/
/*|Inštrukcie pre nastavenie Sigfox Modemu na stránkach LPWAN:          |*/
/*|Callbacks --> NEW --> Custom callback                                |*/
/*|UPLINK, TYPE: DATA, CHANNEL: URL                                     |*/
/*|Do Custom payload config napíšeme: cislo1::float:16 cislo2::uint:16  |*/
/*|Do URL pattern: http://arduino.clanweb.eu/studna/data.php (PRE TEST  |*/
/*|PRIPADNE: (pripadne vas URL webservera s možnostou TLS - SSL)        |*/
/*|V HTTP metóde zvolíme: POST (jediná podporovaná metóda Hladinomeru)  |*/
/*|Do Body (tela správy) doplníme:                                      |*/
/*|dazd={customData#cislo1}&hodnota={customData#cislo2}                 |*/
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

//DEFINICIE, PREMENNE PRE RG-11 - DAŽĎOVÝ SENZOR
#define RG11_Pin 3
#define Bucket_Size 0.01
volatile unsigned long tipCount;     // bucket tip counter used in interrupt routine
volatile unsigned long ContactTime;  // Timer to manage any contact bounce in interrupt routine
long lastCount;
float totalRainfall;
unsigned long timer = 0;


void setup() {
  Sigfox.begin(9600);
  Serial.begin(115200);
  lastCount = 0;
  tipCount = 0;
  totalRainfall = 0;
  Serial.println(F("Hydreon RG-11 Rain Sensor"));
  Serial.print(F("Bucket Size: ")); Serial.print(Bucket_Size); Serial.println(F(" mm"));

  pinMode(RG11_Pin, INPUT);   // set the digital input pin to input for the RG-11 Sensor
  attachInterrupt(digitalPinToInterrupt(RG11_Pin), rgisr, FALLING);     // attach interrupt handler to input pin.
  // we trigger the interrupt on the voltage falling from 5V to GND

  sei();         //Enables interrupts
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
    // we only display the tip count when it has been incremented by the sensor
    cli();         //Disable interrupts
    if (tipCount != lastCount) {
      lastCount = tipCount;
      totalRainfall = tipCount * Bucket_Size;
      Serial.print(F("Tip Count: ")); Serial.print(tipCount);
      Serial.print(F("\tTotal Rainfall: ")); Serial.println(totalRainfall);
    }
    sei();         //Enables interrupts
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
      char sprava[8]; //8B sprava, max mozna 12B
      float cislo1 = totalRainfall;
      unsigned int cislo2 = vzdialenost;
      sprintf(sprava, "%04X%04X", cislo1, cislo2);
      Serial.print(F("Do Sigfox siete odoslany payload: "));
      Serial.print(cislo1);
      Serial.print(", ");
      Serial.print(cislo2);
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

// Interrrupt handler routine that is triggered when the rg-11 detects rain
void rgisr ()   {

  if ((millis() - ContactTime) > 15 ) {  // debounce of sensor signal
    tipCount++;
    ContactTime = millis();
  }
}
// end of rg-11 rain detection interrupt handler
