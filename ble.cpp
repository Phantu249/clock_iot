#include "ble.h"
#include "config.h"
#include <NimBLEDevice.h>

constexpr auto SERVICE_UUID = "AAAA";
constexpr auto CUSTOM_SCREEN_CHARACTERISTIC_UUID = "1001";
constexpr auto TIME_CHARACTERISTIC_UUID = "1002";
constexpr auto BUTTON_CHARACTERISTIC_UUID = "1003";

void Ble::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
  Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());
  isConnected = true;
  NimBLEDevice::getAdvertising()->stop();
}

void Ble::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
  isConnected = false;
  Serial.println("Client disconnected");

  if (canReAdv) {
    Serial.println("Start advertising");
    NimBLEDevice::startAdvertising();
  }
}

Ble::Ble(NimBLECharacteristicCallbacks* onWrite) {
  _frame = 5;  // 100ms per frame

  NimBLEDevice::init("ESP32_Clock");
  _pServer = NimBLEDevice::createServer();
  _pServer->setCallbacks(this);

  _pService = _pServer->createService(SERVICE_UUID);
  _pCharacteristicCustomScreen = _pService->createCharacteristic(
    CUSTOM_SCREEN_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicTime = _pService->createCharacteristic(
    TIME_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicButton = _pService->createCharacteristic(
    BUTTON_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE);

  _pCharacteristicCustomScreen->setCallbacks(onWrite);
  _pCharacteristicTime->setCallbacks(onWrite);
  _pCharacteristicButton->setCallbacks(onWrite);
  _pService->start();

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->enableScanResponse(true);
  pAdvertising->setName("Nimble ESP32_Clock");

  Serial.println("BLE Initialized");
}

void Ble::startAdvertising() {
  canReAdv = true;
  if (!isAdvertising) {
    NimBLEDevice::startAdvertising();
    isAdvertising = true;
    Serial.println("Advertising...");
  }
}

void Ble::stopAdvertising() {
  canReAdv = false;
  if (isAdvertising) {
    NimBLEDevice::stopAdvertising();
    isAdvertising = false;
    Serial.println("Stop Advertising");
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

// Button handlers (default: no action)
void Ble::onButton(Button button, Callback callback) {}
void Ble::onButtonUp() {}
void Ble::onButtonDown() {}
void Ble::onButtonLeft() {}
void Ble::onButtonRight() {}

uint8_t Ble::getFrame() {
  return _frame;
}
