/*|-------------------------------------------------|*/
/*|Projekt: JSON client - ESP32 - Hladinomer        |*/
/*|Popis: Pripojenie k rozhraniu hladinomeru        |*/
/*|Stiahnutie JSON dat, vyparsovanie hodnot, vypis  |*/
/*|Odoslanie dat do dvoch topicov na free MQTT      |*/
/*|Broker pre vývojárov IoT Industies Slovakia      |*/
/*|Autor: Martin Chlebovec                          |*/
/*|E-mail: martinius96@gmail.com                    |*/
/*|Revízia: 9. Marec 2021                           |*/
/*|-------------------------------------------------|*/

#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
const char* ssid = "SSID_NAME"; //SSID vasej WiFi siete
const char* password = "SSID_PASSWORD"; //HESLO vasej WiFi siete
const char* host = "arduino.clanweb.eu"; //IP adresa WiFi termostatu
const char* mqtt_server = "mqtt.iotindustries.sk";
const int httpPort = 80;
WiFiClient klient; //pre HTTP
WiFiClient espClient; //pre MQTT
PubSubClient client(espClient);
unsigned long timer = 0;
int interval = 60000;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Nacitany topic: ");
  Serial.print(topic);
  Serial.print(". Sprava: ");
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
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_Hladinomer_Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("hladinomer/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); //rychlost seriovej linky
  Serial.println();
  Serial.print("Pripajanie na WiFi siet: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //pripoj sa na wifi siet s heslom
  while (WiFi.status() != WL_CONNECTED) { //pokial sa nepripojime na wifi opakuj pripajanie a spustaj funkcie pre ovladanie v offline rezime
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi pripojene s IP:");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.loop();
  if ((millis() - timer) >= interval || timer == 0) {
    timer = millis();
    klient.stop();
    if (klient.connect(host, httpPort)) {
      String url = "/studna_s_prekladom/json_output.php";
      Serial.println("Pripojenie uspesne, nacitavam JSON data");
      klient.print(String("GET ") + url + " HTTP/1.0\r\n" + "Host: " + host + "\r\n" + "User-Agent: NodeMCU\r\n" + "Connection: close\r\n\r\n");
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
      Serial.println("Nepodarilo sa pripojenie k hladinomeru, ani nacitanie JSON dat");
    }
  }
}
