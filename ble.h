#ifndef BLE_H
#define BLE_H

#include "screen.h"
#include <NimBLEDevice.h>

constexpr auto SERVICE_UUID = "AAAA";
constexpr auto CUSTOM_SCREEN_CHARACTERISTIC_UUID = "1001";
constexpr auto TIME_CHARACTERISTIC_UUID = "1002";
constexpr auto ALARM_CHARACTERISTIC_UUID = "1003";
constexpr auto BUTTON_CHARACTERISTIC_UUID = "1004";
constexpr auto WIFI_CHARACTERISTIC_UUID = "1005";
constexpr auto TIMEMODE_CHARACTERISTIC_UUID = "1006";

class Ble : public Screen, public NimBLEServerCallbacks {
protected:
  NimBLEServer* _pServer;
  NimBLEService* _pService;
  NimBLECharacteristic* _pCharacteristicCustomScreen;
  NimBLECharacteristic* _pCharacteristicTime;
  NimBLECharacteristic* _pCharacteristicButton;
  NimBLECharacteristic* _pCharacteristicAlarm;
  NimBLECharacteristic* _pCharacteristicWifi;
  NimBLECharacteristic* _pCharacteristicTimeMode;
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