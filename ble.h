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

public:
  Ble(NimBLECharacteristicCallbacks* onWrite);
  CRGB* draw() override;
  void startAdvertising();
  void stopAdvertising();
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
};

#endif