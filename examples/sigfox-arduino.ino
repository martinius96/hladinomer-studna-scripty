/*|---------------------------------|*/
/*|Projekt: Hladinomer              |*/
/*|Autor: Martin Chlebovec          |*/
/*|E-mail: martinius96@gmail.com    |*/
/*|Web: https://arduino.php5.sk     |*/
/*|Licencia pouzitia: MIT           |*/
/*|---------------------------------|*/

#include <avr\wdt.h>
#include <SoftwareSerial.h>
#include <NewPing.h>
#define pinTrigger    3
#define pinEcho       4
#define maxVzdialenost 450
#define TX 7
#define RX 8
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
SoftwareSerial Sigfox(RX, TX);
void setup() {
  Serial.begin(115200);
  wdt_enable(WDTO_8S);
  Sigfox.begin(9600);
  delay(1000);
}

void loop() {
  wdt_reset();
  if (Sigfox.available()) {
    Serial.write(Sigfox.read());
  }
  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }
  int vzdialenost = sonar.ping_cm();
  delay(50);
  if (vzdialenost > 0) {
    vzdialenost = 0;
    for (int i = 0; i < 5; i++) {
      vzdialenost += sonar.ping_cm();
      delay(50);
    }
    vzdialenost = vzdialenost / 5;
    char sprava[12];
    unsigned int hodnota = (vzdialenost);
    sprintf(sprava, "%04X", hodnota);
    Serial.print("Odosielam cez Sigfox: ");
    Serial.print(hodnota);
    Serial.print(", hexa tvar: ");
    Serial.println(sprava);
    Sigfox.print("AT$SF=");
    Sigfox.println(sprava);
    Serial.print("Vzdialenost medzi senzorom a predmetom je: ");
    Serial.print(vzdialenost);
    Serial.println(" cm.");
  }
  else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
    wdt_reset();
    delay(500);
  }
/*|--------------------------------|*/
/*|NUTNE NASTAVIT SIGFOX CALLBACK  |*/
/*|ODOSLANIE DO DATABAZY, REST API |*/
/*|--------------------------------|*/
  for (int i = 0; i <= 300; i++) {
    delay(1000);
    wdt_reset();
  }
}
