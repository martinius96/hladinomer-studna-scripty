/*|---------------------------------------------------------------------|*/
/*|Projekt: Hladinomer - Arduino + Ethernet W5100                       |*/
/*|Autor: Martin Chlebovec                                              |*/
/*|E-mail: martinius96@gmail.com                                        |*/
/*|Licencia pouzitia: MIT                                               |*/
/*|Revízia: 06. Október 2020                                            |*/
/*|---------------------------------------------------------------------|*/

//HLAVICKOVE SUBORY PRE ARDUINO A ETHERNET SHIELD + watchdog
#include <avr\wdt.h>
#include <SPI.h>
#include <Ethernet.h>

//PREMENNE, HLAVICKOVY SUBOR, OBJEKT PRE HC-SR04 / JSN-SR04T
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

unsigned long timer = 0;


void setup() {
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
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

  if ((millis() - timer) >= 300000 || timer == 0) { //rutina raz za 5 minut
    timer = millis();
    if (Ethernet.begin(mac) == 0) {
      Serial.println(F("Failed to configure Ethernet using DHCP"));
      Ethernet.begin(mac);
      //Ethernet.begin(mac, ip);
      //Ethernet.begin(mac, ip, dns);
      //Ethernet.begin(mac, ip, dns, gateway);
      //Ethernet.begin(mac, ip, dns, gateway, subnet);
      wdt_reset();
    }
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
      String data = "hodnota=" + String(vzdialenost);
      String url = "/studna/data.php";
      client.stop(); //ukoncenie vsetkych existujucich spojeni
      if (client.connect(host, 80)) {
        client.println("POST " + url + " HTTP/1.0");
        client.println("Host: " + (String)host);
        client.println(F("User-Agent: EthernetW5100"));
        client.println(F("Connection: close"));
        client.println(F("Content-Type: application/x-www-form-urlencoded;"));
        client.print(F("Content-Length: "));
        client.println(data.length());
        client.println();
        client.println(data);
        Serial.println(F("Data uspesne odoslane na web"));
      } else {
        Serial.println(F("Pripojenie zlyhalo..."));
      }
      client.stop();
    } else {
      Serial.println(F("Vzdialenost medzi predmetom a senzorom je mimo rozsah."));
    }
  }
}
