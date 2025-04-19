#ifndef MENU_H
#define MENU_H

#include "screen.h"

typedef enum : uint8_t {
  TIME_SETTING,
  BLUETOOTH_SETTING,
  GAME1,
  MENU_OPTION_COUNT
} MENU_OPTION;

typedef enum : uint8_t {
  UP,
  DOWN
} SLIDE_DIRECTION;

class Menu : public Screen {
protected:
  MENU_OPTION _currentOption;
  MENU_OPTION _nextOption;
  bool _isSliding;
  float _slideOffset;
  SLIDE_DIRECTION _slideDirection;
  unsigned long lastUpdate = 0; // Thời gian lần gọi cuối (ms)
  const float slideSpeed = 30.0f; // Tốc độ trượt: 16 pixel/giây (tùy chỉnh)

public:
  Menu();
  CRGB* draw() override;
  void renderFrame(MENU_OPTION option, CRGB* frame);
  void onButton(Button button, Callback callback = NULL) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonMenu(Callback callback);
  int getIndex(uint8_t x, uint8_t y);
};

#endif