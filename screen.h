#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "config.h"
#include <FastLED.h>

typedef enum : uint8_t {
  BUTTON_UP = 35,
  BUTTON_DOWN = 36,
  BUTTON_LEFT = 37,
  BUTTON_RIGHT = 38
} Button;

typedef void (*Callback)(void*);

class Screen {
  protected:
    uint8_t _frame;

  public:   
    virtual CRGB* draw();   
    virtual void onButton(Button button,  Callback callback = NULL);
    virtual void onButtonUp();
    virtual void onButtonDown();
    virtual void onButtonLeft() ;
    virtual void onButtonRight();
    virtual uint8_t getFrame();
};
#endif