#ifndef MENU_H
#define MENU_H

#include "screen.h"

typedef enum : uint8_t {
  TIME_SETTING,
  BLUETOOTH_SETTING,
  GAME1,
  MENU_OPTION_COUNT
} MenuOption;

typedef enum : uint8_t {
  UP,
  DOWN
} SlideDirection;

class Menu : public Screen {
protected:
  MenuOption _currentOption;
  MenuOption _nextOption;
  bool _isSliding;
  float _slideOffset;
  SlideDirection _slideDirection;
  unsigned long lastUpdate = 0; // Thời gian lần gọi cuối (ms)
  const float slideSpeed = 30.0f; // Tốc độ trượt: 16 pixel/giây (tùy chỉnh)

public:
  Menu();
  CRGB* draw() override;
  void renderFrame(MenuOption option, CRGB* frame);
  void onButton(Button button, Callback callback = NULL) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonLeft() override;
  void onButtonRight() override;
  void onButtonRight(Callback callback);
  int getIndex(uint8_t x, uint8_t y);
};

#endif