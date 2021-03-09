/*|--------------------------------------------------------|*/
/*|Projekt: JSON client - Arduino + Ethernet Wiznet, etc.. |*/
/*|Popis: Pripojenie k rozhraniu Hladinomera               |*/
/*|Stiahnutie JSON dat, vyparsovanie hodnot, vypis         |*/
/*|Odoslanie dat do dvoch topicov na free MQTT             |*/
/*|Broker pre vývojárov IoT Industies Slovakia             |*/
/*|Autor: Martin Chlebovec                                 |*/
/*|E-mail: martinius96@gmail.com                           |*/
/*|Revízia: 9. Marec 2021                                  |*/
/*|--------------------------------------------------------|*/

//#include <Ethernet.h>    //Wiznet W5100
#include <Ethernet2.h>     //Wiznet W5500
//#include <Ethernet3.h>   //Wiznet W5500 V2
//#include <UIPEthernet.h> //ENC28J60
#include <ArduinoJson.h>
#include <PubSubClient.h>
const char* host = "arduino.clanweb.eu"; //IP adresa / domena webservera
const char* mqtt_server = "mqtt.iotindustries.sk";
const int httpPort = 80;
byte mac[] = { 0xAA, 0xCA, 0xCC, 0x81, 0xBB, 0xAC };
IPAddress ip(192, 168, 1, 254); //staticke pridelenie IP
EthernetClient klient;
unsigned long timer = 0;
int interval = 60000;
EthernetClient espClient; //pre MQTT
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print(F("Message arrived on topic: "));
  Serial.print(topic);
  Serial.print(F(". Message: "));
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("Ethernet_Arduino_Client")) {
      Serial.println(F("connected"));
      // Subscribe
      client.subscribe("hladinomer/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  if (Ethernet.begin(mac) == 0) {
    Ethernet.begin(mac); //nechame pridelenie na DHCP server
    // Ethernet.begin(mac, ip); //nastavime manualne
  }
  Serial.print(F("DHCP assigned IP: "));
  Serial.println(Ethernet.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  Serial.println(F("Ready"));
}

void loop() {
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    klient.stop();
    if (klient.connect(host, httpPort)) {
      String url = "/studna_s_prekladom/json_output.php";
      Serial.println(F("Pripojenie uspesne, nacitavam JSON data"));
      klient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: W5500\r\n" + "Connection: close\r\n\r\n");
      while (klient.connected()) {
        String line = klient.readStringUntil('\n'); //HTTP HEADER
        //Serial.println(line);
        if (line == "\r") {
          break;
        }
      }
      DynamicJsonDocument doc(128);
      String line = klient.readString(); //PAYLOAD
      Serial.println(line);
      deserializeJson(doc, line);
      JsonObject obj = doc.as<JsonObject>();
      int vyska = obj[String("value")];
      float objem = obj[String("volume")];
      Serial.print("Vyska hladiny: ");
      Serial.print(vyska);
      Serial.println(" cm");
      char pole_vyska[32];
      dtostrf(vyska, 1, 2, pole_vyska);
      client.publish("hladinomer/vyska", pole_vyska);
      Serial.print("Objem studne: ");
      Serial.print(objem);
      Serial.println(" litrov");
      char pole_objem[32];
      dtostrf(objem, 1, 2, pole_objem);
      client.publish("hladinomer/objem", pole_objem);
    } else if (!klient.connect(host, httpPort)) {
      Serial.println(F("Nepodarilo sa pripojenie ku hladinomeru, ani nacitanie JSON data"));
    }
  }
}
