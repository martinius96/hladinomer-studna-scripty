/*|-------------------------------------------------|*/
/*|Projekt: Hladinomer - Arduino + Ethernet W5500   |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Web: https://arduino.php5.sk                     |*/
/*|Licencia pouzitia: MIT                           |*/
/*|Revízia: 26. Marec 2020                          |*/
/*|-------------------------------------------------|*/

#include <avr\wdt.h>
#include <SPI.h>
#include <Ethernet2.h>
#include <NewPing.h>
#define pinTrigger    5
#define pinEcho       6
#define maxVzdialenost 450
NewPing sonar(pinTrigger, pinEcho, maxVzdialenost);
char* host = "arduino.clanweb.eu";
byte mac[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
//IPAddress ip(192, 168, 0, 2);
//IPAddress dnServer(192, 168, 0, 1);
//IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);
EthernetClient client;
void setup() {
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac);
    //Ethernet.begin(mac, ip);
    //Ethernet.begin(mac, ip, dns);
    //Ethernet.begin(mac, ip, dns, gateway);
    //Ethernet.begin(mac, ip, dns, gateway, subnet);
  }
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac);
    //Ethernet.begin(mac, ip);
    //Ethernet.begin(mac, ip, dns);
    //Ethernet.begin(mac, ip, dns, gateway);
    //Ethernet.begin(mac, ip, dns, gateway, subnet);
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
    Serial.print("Vzdialenost medzi senzorom a predmetom je: ");
    Serial.print(vzdialenost);
    Serial.println(" cm.");
    String data = "hodnota=" + String(vzdialenost);
    String url = "/studna/data.php";
    if (client.connect(host, 80)) {
      client.println("POST " + url + " HTTP/1.0");
      client.println("Host: " + (String)host);
      client.println("User-Agent: EthernetW5500");
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
