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
  int _slideOffset;
  SLIDE_DIRECTION _slideDirection;

public:
  Menu();
  CRGB* draw() override;
  void renderFrame(MENU_OPTION option, CRGB* frame);
  void onButton(Button button, Callback callback = NULL) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonLeft() override;
  void onButtonRight() override;
  void onButtonRight(Callback callback);
  int getIndex(uint8_t x, uint8_t y);
};

#endif