#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// --- CONFIGURATION ---
const char* ssid = "ENTER_SSID_HERE";
const char* password = "ENTER_PASSWORD_HERE";
const char* url = "https://hladinomer.eu/data.php"; // HTTPS server
const char* token = "123456789";

// --- HC-SR04 PINS ---
const int TRIG_PIN = 3;
const int ECHO_PIN = 2;

// --- WIFI CONNECTION ---
void connectWiFi() {
  Serial1.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  Serial1.println("\nConnected!");
  Serial1.print("IP Address: ");
  Serial1.println(WiFi.localIP());
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
        Serial1.print("Status: ");
        Serial1.println(httpResponseCode);
        Serial1.print("Response: ");
        Serial1.println(response);
      } else {
        Serial1.print("Error sending data: ");
        Serial1.println(httpResponseCode);
      }
      https.end();
    } else {
      Serial1.println("Unable to connect to server!");
    }
  } else {
    Serial1.println("WiFi not connected!");
  }
}

// --- SETUP ---
void setup() {
  Serial1.begin(115200);
  Serial1.println("UART running");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectWiFi();
}

// --- MAIN LOOP ---
void loop() {
  float distance = measureDistance();
  Serial1.print("Distance: ");
  Serial1.print(distance);
  Serial1.println(" cm");

  sendData(distance);

  delay(300000); // wait 5 minutes
}
