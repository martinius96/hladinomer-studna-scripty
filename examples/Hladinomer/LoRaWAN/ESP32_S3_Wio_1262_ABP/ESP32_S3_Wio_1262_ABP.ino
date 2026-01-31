/*|-------------------------------------------------------------------------------|*/
/*|Water level LoRaWAN node based on - ESP32-S3 XIAO /w Wio-SX1262 Seeed Studio   |*/
/*|Usage with ultrasonic distance sensor HC-SR04 / JSN-SR04T                      |*/
/*|Compatible with The Things Network (TTN), Frequency plan: 868.1 MHz, SF9       |*/
/*|Author: Martin Chlebovec (martinius96)                                         |*/
/*|E-mail: martinius96@gmail.com                                                  |*/
/*|Test web interface: https://hladinomer.eu/?lang=en                             |*/
/*|Revision: 31. Jan. 2026                                                        |*/
/*|-------------------------------------------------------------------------------|*/

#include "config.h"
#include "EEPROM.h"
#include <NewPingESP8266.h>
// regional choices: EU868, US915, AU915, AS923, IN865, KR920, CN780, CN500
const LoRaWANBand_t Region = EU868;
const uint8_t subBand = 0; // For US915 and AU915
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  300
#define pinTrigger    6 //SCL
#define pinEcho       5 //SDA
#define maxVzdialenost 450
RTC_DATA_ATTR unsigned long BootCount;
NewPingESP8266 sonar(pinTrigger, pinEcho, maxVzdialenost);

// SX1262 pin order: Module(NSS/CS, DIO1, RESET, BUSY);
SX1262 radio = new Module(41, 39, 42, 40);

// create the LoRaWAN node
LoRaWANNode node(&radio, &Region, subBand);

uint32_t devAddr =      {  RADIOLIB_LORAWAN_DEV_ADDR };
uint8_t fNwkSIntKey[] = { RADIOLIB_LORAWAN_FNWKSINT_KEY };
uint8_t sNwkSIntKey[] = { RADIOLIB_LORAWAN_SNWKSINT_KEY };
uint8_t nwkSEncKey[] =  { RADIOLIB_LORAWAN_NWKSENC_KEY };
uint8_t appSKey[] =     { RADIOLIB_LORAWAN_APPS_KEY };

#define LORAWAN_DEV_INFO_SIZE 36
uint8_t deviceInfo[LORAWAN_DEV_INFO_SIZE] = {0};

#define SERIAL_DATA_BUF_LEN  64
uint8_t serialDataBuf[SERIAL_DATA_BUF_LEN] = {0};
uint8_t serialIndex = 0;

#define UPLINK_PAYLOAD_MAX_LEN  256
uint8_t uplinkPayload[UPLINK_PAYLOAD_MAX_LEN] = {0};
uint16_t uplinkPayloadLen = 0;

uint32_t previousMillis = 0;

/*void deviceInfoLoad() {
  uint16_t checkSum = 0, checkSum_ = 0;
  for (int i = 0; i < LORAWAN_DEV_INFO_SIZE; i++) deviceInfo[i] = EEPROM.read(i);
  for (int i = 0; i < 32; i++) checkSum += deviceInfo[i];
  memcpy((uint8_t *)(&checkSum_), deviceInfo + 32, 2);

  if (checkSum == checkSum_)
  {
    memcpyr((uint8_t *)(&joinEUI), deviceInfo, 8);
    memcpyr((uint8_t *)(&devEUI), deviceInfo + 8, 8);
    memcpy(appKey, deviceInfo + 16, 16);

    Serial.println("Load device info:");
    Serial.print("JoinEUI:");
    Serial.println(joinEUI, HEX);
    Serial.print("DevEUI:");
    Serial.println(devEUI, HEX);
    Serial.print("AppKey:");
    arrayDump(appKey, 16);
    Serial.print("nwkKey:");
    arrayDump(nwkKey, 16);
  }
  else
  {
    Serial.println("Use the default device info as LoRaWAN param");
  }
  }

  void deviceInfoSet() {
  if (Serial.available())
  {
    serialDataBuf[serialIndex++] = Serial.read();
    if (serialIndex >= SERIAL_DATA_BUF_LEN) serialIndex = 0;
    if (serialIndex > 2 && serialDataBuf[serialIndex - 2] == '\r' && serialDataBuf[serialIndex - 1] == '\n')
    {
      Serial.println("Get serial data:");
      arrayDump(serialDataBuf, serialIndex);
      if (serialIndex == 34) // 8 + 8 + 16 + 2
      {
        uint16_t checkSum = 0;
        for (int i = 0; i < 32; i++) checkSum += serialDataBuf[i];
        memcpy(deviceInfo, serialDataBuf, 32);
        memcpy(deviceInfo + 32, (uint8_t *)(&checkSum), 2);
        for (int i = 0; i < 34; i++) EEPROM.write(i, deviceInfo[i]);
        EEPROM.commit();
        Serial.println("Save serial data, please reboot...");
      }
      else
      {
        Serial.println("Error serial data length");
      }

      serialIndex = 0;
      memset(serialDataBuf, sizeof(serialDataBuf), 0);
    }
  }
  }
*/

void setup() {
  Serial.begin(115200);
  delay(5000);  // Give time to switch to the serial monitor
  Serial.println(F("\nSetup ... "));

  Serial.println(F("Initialise the radio"));
  int state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, F("Initialise radio failed"), state, true);
  // SX1262 rf switch order: setRfSwitchPins(rxEn, txEn);
  radio.setRfSwitchPins(38, RADIOLIB_NC);
  Serial.println(F("Initialise LoRaWAN Network credentials"));
  node.beginABP(devAddr, fNwkSIntKey, sNwkSIntKey, nwkSEncKey, appSKey);
  node.activateABP();
  delay(500);
  Serial.println(F("Ready!\n"));
  // Voliteľné – užitočné nastavenia (môžeš zmeniť podľa potreby)
  node.setADR(false);                    // vypnúť adaptívny datarate
  node.setDatarate(LORAWAN_UPLINK_DATA_RATE);                   // EU868: DR3 = SF9 / BW125 → bežná hodnota pre TTN

  node.setDutyCycle(false);              // pre testovanie vypnúť duty-cycle (pozor na TTN fair use!)

  Serial.println(F("LoRaWAN ABP node ready\n"));
}

void loop() {
  uint16_t distance = sonar.ping_cm();
  delay(50);
  if (distance > 0) {
    distance = 0;
    for (int i = 0; i < 10; i++) {
      distance += sonar.ping_cm();
      delay(50);
    }
    distance = distance / 10;
    Serial.print(F("Distance is: "));
    Serial.print(distance);
    Serial.println(F(" cm."));
    uplinkPayloadLen = 0;
    uplinkPayload[uplinkPayloadLen++] = (distance >> 8) & 0xFF; // MSB
    uplinkPayload[uplinkPayloadLen++] = distance & 0xFF;        // LSB
    Serial.print("Uplink payload length: ");
    Serial.println(uplinkPayloadLen);
    // Output the uplink payload for debugging
    Serial.print("Uplink payload: ");
    for (int i = 0; i < uplinkPayloadLen; i++) {
      Serial.print(uplinkPayload[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    int16_t state = node.sendReceive(uplinkPayload, uplinkPayloadLen, LORAWAN_UPLINK_USER_PORT);
    if (state != RADIOLIB_LORAWAN_NO_DOWNLINK && state != RADIOLIB_ERR_NONE) {
      Serial.println("Error in sendReceive:");
      Serial.println(state);
    } else {
      Serial.println("Sending uplink successful!");
    }
    delay(1000);
    if (BootCount < 1) {
      delay(30000);
    }
    BootCount++;
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
    esp_deep_sleep_start();
  }
  else {
    Serial.println(F("Distance between ultrasonic sensor and water level is out of range."));
  }
}
