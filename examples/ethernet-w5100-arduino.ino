/*|-------------------------------------------------|*/
/*|Projekt: Hladinomer - Arduino + Ethernet W5100   |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Web: https://arduino.php5.sk                     |*/
/*|Licencia pouzitia: MIT                           |*/
/*|-------------------------------------------------|*/

#include <avr\wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <NewPing.h>
#define pinTrigger    3
#define pinEcho       4
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char* host = "www.arduino.php5.sk";
IPAddress ip(192, 168, 1, 101);
EthernetClient client;
void setup() {
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac);
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
    String data = "hodnota=" + String(vzdialenost);
    String url = "/studna/data.php";
    if (client.connect(host, 80)) {
      client.println("POST " + url + " HTTP/1.0");
      client.println("Host: " + (String)host);
      client.println("User-Agent: EthernetW5100");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded;");
      client.print("Content-Length: ");
      client.println(data.length());
      client.println();
      client.println(data);
      Serial.println("Data uspesne odoslane na web");
    } else {
      Serial.println("Pripojenie zlyhalo...");
    }
    client.stop();
    for (int i = 0; i <= 300; i++) {
      delay(1000);
      wdt_reset();
    }
  } else {
    Serial.println("Vzdialenost medzi predmetom a senzorom je mimo rozsah.");
    wdt_reset();
    delay(500);
  }
}
