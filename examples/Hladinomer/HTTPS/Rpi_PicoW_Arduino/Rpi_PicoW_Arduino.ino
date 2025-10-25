#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// --- CONFIGURATION ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* url = "https://hladinomer.eu/data.php"; // HTTPS server
const char* token = "123456789";

// --- HC-SR04 PINS ---
const int TRIG_PIN = 3;
const int ECHO_PIN = 2;

// --- UART Client ---
#define UART Serial1

// --- WIFI CONNECTION ---
void connectWiFi() {
  UART.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    UART.print(".");
  }
  UART.println("\nConnected!");
  UART.print("IP Address: ");
  UART.println(WiFi.localIP());
}

// --- MEASURE DISTANCE ---
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  float distance_cm = (duration * 0.0343) / 2.0;
  return distance_cm;
}

// --- SEND DATA TO SERVER ---
void sendData(float value) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // skip SSL verification (for testing)
    HTTPClient https;

    String payload = "hodnota=" + String(value) + "&token=" + String(token);

    if (https.begin(client, url)) {
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");
      https.addHeader("User-Agent", "PicoW-MCU/1.0");

      int httpResponseCode = https.POST(payload);

      if (httpResponseCode > 0) {
        String response = https.getString();
        UART.print("Status: ");
        UART.println(httpResponseCode);
        UART.print("Response: ");
        UART.println(response);
      } else {
        UART.print("Error sending data: ");
        UART.println(httpResponseCode);
      }
      https.end();
    } else {
      UART.println("Unable to connect to server!");
    }
  } else {
    UART.println("WiFi not connected!");
  }
}

// --- SETUP ---
void setup() {
  UART.begin(115200);
  UART.println("UART running");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectWiFi();
}

// --- MAIN LOOP ---
void loop() {
  float distance = measureDistance();
  UART.print("Distance: ");
  UART.print(distance);
  UART.println(" cm");

  sendData(distance);

  delay(300000); // wait 5 minutes
}
