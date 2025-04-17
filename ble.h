#ifndef BLE_H
#define BLE_H

#include "screen.h"
#include <NimBLEDevice.h>

class Ble : public Screen, public NimBLEServerCallbacks {
protected:
  NimBLEServer* _pServer;
  NimBLEService* _pService;
  NimBLECharacteristic* _pCharacteristicCustomScreen;
  NimBLECharacteristic* _pCharacteristicTime;
  NimBLECharacteristic* _pCharacteristicButton;
  NimBLEAdvertising* pAdvertising;
  bool isAdvertising = false;
  bool canReAdv = false;
  bool isConnected = false;
  CRGB frame[NUM_LEDS];

public:
  Ble(NimBLECharacteristicCallbacks* onWrite);
  CRGB* draw() override;
  void onButton(Button button, Callback callback = NULL) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonLeft() override;
  void onButtonRight() override;
  uint8_t getFrame() override;
  void startAdvertising();
  void stopAdvertising();
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
};

#endif