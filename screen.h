#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "config.h"
#include <FastLED.h>

#ifndef NUM_LEDS
#define NUM_LEDS 256
#endif

typedef enum : uint8_t {
  BUTTON_UP = 35,
  BUTTON_DOWN = 36,
  BUTTON_LEFT = 37,
  BUTTON_RIGHT = 38,
  BUTTON_BACK = 39,
  BUTTON_MENU = 40
} Button;

typedef void (*Callback)(void*, State newState);

class Screen {
protected:
  uint8_t _frame;
  CRGB frame[NUM_LEDS];

public:
  virtual ~Screen() = default;

  virtual CRGB* draw() = 0;
  virtual void onButton(Button button, Callback callback = NULL);
  virtual void onButtonUp();
  virtual void onButtonDown();
  virtual void onButtonLeft();
  virtual void onButtonRight();
  virtual void onButtonBack();
  virtual void onButtonMenu();
  virtual uint8_t getFrame() {
    return _frame;
  }
};

inline void Screen::onButton(Button, Callback) {}
inline void Screen::onButtonUp() {}
inline void Screen::onButtonDown() {}
inline void Screen::onButtonLeft() {}
inline void Screen::onButtonRight() {}
inline void Screen::onButtonBack() {}
inline void Screen::onButtonMenu() {}

#endif