#include <Wire.h>
#include <VL53L1X.h>
#include <NimBLEDevice.h>

VL53L1X sensor;

#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-5678-90ab-cdef-1234567890ab"

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;

class ServerCallbacks : public NimBLEServerCallbacks {
public:
  void onConnect(NimBLEServer* pServer) {
    Serial.println("Client connected");
  }

  void onDisconnect(NimBLEServer* pServer) {
    Serial.println("Client disconnected");
    // advertising už beží stále
  }
};

void setup() {
  Serial.begin(115200);

  // --- VL53L1X ---
  Wire.begin(22, 23);
  Wire.setClock(400000);
  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(200);

  // --- BLE ---
  NimBLEDevice::init("ESP32C6_Hladinomer");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService* pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::NOTIFY
  );
  pCharacteristic->setValue("0");
  pService->start();

  NimBLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  NimBLEAdvertisementData scanResponseData;
  scanResponseData.setName("ESP32C6_Hladinomer");
  pAdvertising->setScanResponseData(scanResponseData);

  pAdvertising->start();

  Serial.println("BLE server running, advertising always visible");
}

void loop() {
  int vzdialenost = sensor.read() / 10;
  if (sensor.timeoutOccurred()) {
    Serial.println("Sensor timeout!");
    return;
  }

  Serial.print("Distance: ");
  Serial.print(vzdialenost);
  Serial.println(" cm");

  String value = String(vzdialenost);
  pCharacteristic->setValue(value);
  pCharacteristic->notify();

  delay(1000);
}
