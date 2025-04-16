#ifndef MENU_H
#define MENU_H

#include "screen.h"

typedef enum : uint8_t {
  TIME_SETTING,
  BLUETOOTH_SETTING,
  GAME1,
  MENU_OPTION_COUNT 
} MENU_OPTION;

class Menu : public Screen {
  protected:
    CRGB _frames[MENU_OPTION_COUNT][NUM_LEDS];  // Mỗi frame là 1 mảng led
    MENU_OPTION _currentOption = TIME_SETTING;

    CRGB _slideFrame[NUM_LEDS];
    int8_t _slideOffset = 0;
    bool _isSliding = false;
    MENU_OPTION _nextOption;

  public:
    Menu();
    CRGB* draw() override;
    void onButton(Button button, Callback callback = NULL) override;
    void onButtonUp() override;
    void onButtonDown() override;
    void onButtonLeft() override;
    void onButtonRight() override;
    void onButtonRight(Callback callback);
    uint8_t getFrame() override;
    int getIndex(uint8_t x, uint8_t y);
};

#endif