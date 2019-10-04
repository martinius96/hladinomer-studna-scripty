/*|---------------------------------|*/
/*|Projekt: Hladinomer              |*/
/*|Autor: Martin Chlebovec          |*/
/*|E-mail: martinius96@gmail.com    |*/
/*|Web: https://arduino.php5.sk     |*/
/*|Licencia pouzitia: MIT           |*/
/*|---------------------------------|*/

#include <avr\wdt.h>
#include <SPI.h>
#include <LoRa.h>
#include <NewPing.h>
#define pinTrigger    3
#define pinEcho       4
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
void setup() {
  Serial.begin(115200);
  wdt_enable(WDTO_8S);
  if (!LoRa.begin(915E6)) {
    Serial.println("Lora modul nenajdeny! Cakaj na Watchdog reset! Vykona sa o 8 sekund");
    while (1);
  }
}

void loop() {
  wdt_reset();
  int vzdialenost = sonar.ping_cm();
  delay(50);
  if (vzdialenost > 0) {
    vzdialenost = 0;
    for (int i = 0; i < 5; i++) {
      vzdialenost += sonar.ping_cm();
      delay(50);
    }
    vzdialenost = vzdialenost / 5;
    LoRa.beginPacket();
    LoRa.print(vzdialenost);
    LoRa.endPacket();
    Serial.print("Vzdialenost medzi senzorom a predmetom je: ");
    Serial.print(vzdialenost);
    Serial.println(" cm.");
  }
  else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
    wdt_reset();
    delay(500);
  }
  for (int i = 0; i <= 300; i++) {
    delay(1000);
    wdt_reset();
  }
}
