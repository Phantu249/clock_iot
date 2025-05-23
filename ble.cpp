#include "ble.h"
#include "config.h"
#include <NimBLEDevice.h>

void Ble::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
  Serial.printf("\n[INFO]: Client address: %s", connInfo.getAddress().toString().c_str());
  isConnected = true;
  NimBLEDevice::getAdvertising()->stop();
}

void Ble::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
  isConnected = false;
  Serial.print("\n[INFO]: Client disconnected");

  if (canReAdv) {
    Serial.print("\n[INFO]: Start advertising");
    NimBLEDevice::startAdvertising();
  }
}

Ble::Ble(NimBLECharacteristicCallbacks* onWrite) {
  _frame = 3;  // 100ms per frame

  NimBLEDevice::init(BLE_NAME);
  _pServer = NimBLEDevice::createServer();
  _pServer->setCallbacks(this);

  _pService = _pServer->createService(SERVICE_UUID);
  _pCharacteristicCustomScreen = _pService->createCharacteristic(
    CUSTOM_SCREEN_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicTime = _pService->createCharacteristic(
    TIME_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicButton = _pService->createCharacteristic(
    BUTTON_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicAlarm = _pService->createCharacteristic(
    ALARM_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicWifi = _pService->createCharacteristic(
    WIFI_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

  _pCharacteristicTimeMode = _pService->createCharacteristic(
    TIMEMODE_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicCustomScreen->setCallbacks(onWrite);
  _pCharacteristicTime->setCallbacks(onWrite);
  _pCharacteristicButton->setCallbacks(onWrite);
  _pCharacteristicTimeMode->setCallbacks(onWrite);
  _pCharacteristicWifi->setCallbacks(onWrite);
  _pCharacteristicAlarm->setCallbacks(onWrite);

  _pService->start();

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->enableScanResponse(true);
  pAdvertising->setName(BLE_NAME);

  Serial.print("\n[INFO]: BLE Initialized");
}

void Ble::startAdvertising() {
  canReAdv = true;
  if (!isAdvertising) {
    NimBLEDevice::startAdvertising();
    isAdvertising = true;
    Serial.print("\n[INFO]: Advertising...");
  }
}

void Ble::stopAdvertising() {
  canReAdv = false;
  if (isAdvertising) {
    NimBLEDevice::stopAdvertising();
    isAdvertising = false;
    Serial.print("\n[INFO]: Stop Advertising");
  }
}

CRGB* Ble::draw() {
  if (!isConnected) {
    fill_solid(frame, NUM_LEDS, CRGB::Black);  // Tối ưu dùng FastLED

    static uint8_t blinkCounter = 0;
    blinkCounter++;

    if (blinkCounter % 5 == 0) {
      for (uint8_t y = 6; y < 10; y++) {
        for (uint8_t x = 6; x < 10; x++) {
          int index = y * NUM_COLS + x;
          frame[index] = CRGB::Blue;
        }
      }
    }
  } else {
    for (uint8_t y = 6; y < 10; y++) {
      for (uint8_t x = 6; x < 10; x++) {
        int index = y * NUM_COLS + x;
        frame[index] = CRGB::Blue;
      }
    }
  }

  return frame;
}
