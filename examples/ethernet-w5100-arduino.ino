/*|------------------------------------|*/
/*|Projekt: Hladinomer                 |*/
/*|Autor: Martin Chlebovec             |*/
/*|E-mail: martinius96@gmail.com       |*/
/*|Web: https://arduino.php5.sk/studna |*/
/*|Licencia pouzitia: MIT              |*/
/*|------------------------------------|*/

#include <avr\wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <NewPing.h>
#define pinTrigger    3
#define pinEcho       4
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "www.arduino.php5.sk";
IPAddress ip(192, 168, 1, 101);
EthernetClient client;
void setup() {
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  wdt_enable(WDTO_8S);
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
    Serial.print("Vzdialenost medzi senzorom a predmetom je: ");
    Serial.print(vzdialenost);
    Serial.println(" cm.");
    if (client.connect(server, 80)) {
      String hodnota = String(vzdialenost);
      Serial.println("Pripojenie uspesne na webserver, vykonavam request... ");
      client.print("GET /studna/data.php?hodnota=");
      client.print(hodnota);
      client.println(" HTTP/1.1");
      client.println("Host: www.arduino.php5.sk");
      client.println("Connection: close");
      client.println();
      Serial.println("Data uspesne odoslane na web");
      client.stop();
    } else {
      Serial.println("Pripojenie zlyhalo...");
    }
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
