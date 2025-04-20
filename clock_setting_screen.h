#ifndef CLOCK_SETTING_SCREEN_H
#define CLOCK_SETTING_SCREEN_H

#include "screen.h"
#include <time.h>

typedef enum : uint8_t {
  SETTING_HOUR,
  SETTING_MINUTE,
  SETTING_DONE
} SettingState;

class ClockSettingScreen : public Screen {
private:
  struct tm timeinfo;
  uint32_t& timestamp; // Reference to global UTC timestamp
  int8_t offset;      // UTC offset in hours
  SettingState settingState;
  bool blinkState;
  uint32_t lastBlinkTime;
  static const uint16_t BLINK_INTERVAL = 500; // 1s blink interval
  static const uint8_t MATRIX_WIDTH = 16;
  static const uint8_t MATRIX_HEIGHT = 16;

  uint16_t getLedIndex(uint8_t x, uint8_t y);
  void drawDigit(uint8_t digit, uint8_t x, uint8_t y, CRGB color);
  void drawTime();

public:
  ClockSettingScreen(uint32_t& timestamp, int8_t offset);
  CRGB* draw() override;
  void onButton(Button button, Callback callback) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonLeft() override;
  void onButtonRight() override;
  void onButtonBack() override;
  void onButtonMenu() override;
};

#endif