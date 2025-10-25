#include <LoRa.h>
#include "mbedtls/aes.h"

//Ultrasonic pinout
#define pinTrigger    7
#define pinEcho       6

//Semtech pinout
#define SS 5
#define RST 14
#define DI0 2

#define maxdistance 450

NewPingESP8266 sonar(pinTrigger, pinEcho, maxdistance);

unsigned long timer = 0;
#define AES_BLOCK_SIZE 16

//16 bytes key
const uint8_t aes_key[16] = {
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};

//Data structure
struct DataPacket {
  unsigned int distance;
};

void aes_encrypt_ecb(const uint8_t *input, uint8_t *output, size_t length, const uint8_t *key) {
  mbedtls_aes_context aes;
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, key, 128);

  // Šifrujeme po blokoch po 16 bajtoch
  for (size_t i = 0; i < length; i += AES_BLOCK_SIZE) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input + i, output + i);
  }
  mbedtls_aes_free(&aes);
}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa started");

  LoRa.setTxPower(8);
  LoRa.setSpreadingFactor(9);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
}

void loop() {
  if ((millis() - timer) >= 600000 || timer == 0) {
    timer = millis();

    unsigned int distance = sonar.ping_cm();
    delay(50);
    if (distance > 0) {
      distance = 0;
      for (int i = 0; i < 10; i++) {
        distance += sonar.ping_cm();
        delay(50);
      }
      distance = distance / 10;

      Serial.print(F("Distance between water level and sensor is: "));
      Serial.print(distance);
      Serial.println(F(" cm."));
      Serial.println(F("Sending encrypted data..."));

      DataPacket packet;
      packet.distance = distance;

      // Buffer na 16 bajtov (jeden AES blok)
      uint8_t plain_block[AES_BLOCK_SIZE] = {0};
      uint8_t encrypted_block[AES_BLOCK_SIZE];

      // Skopíruj dáta do bufferu a zvyšok nechaj padding = 0
      memcpy(plain_block, &packet, sizeof(packet));

      // Šifrovanie
      aes_encrypt_ecb(plain_block, encrypted_block, AES_BLOCK_SIZE, aes_key);

      // Odoslanie zašifrovaného bloku
      LoRa.beginPacket();
      LoRa.write(encrypted_block, AES_BLOCK_SIZE);
      LoRa.endPacket();

      Serial.println("Encrypted packet sent");
    } else {
      Serial.println(F("Distance out of range, retrying..."));
      timer = 0; // reset timer, skúsiť znova
    }
    LoRa.sleep();
  }
}
