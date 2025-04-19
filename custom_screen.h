#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

#include "screen.h"
#include "config.h"
#include <Preferences.h>
#include <FastLED.h>

#ifndef MAX_FRAME
#define MAX_FRAME 5
#endif

class CustomScreen : public Screen {
protected:
  uint8_t frames[MAX_FRAME][NUM_LEDS];
  uint8_t currentFrame = 0;
  uint8_t totalFrame = 0;
  Preferences preferences;
  bool isGif = false;

public:
  CustomScreen();
  CRGB* draw() override;
  void renderFrame();
  int getIndex(uint8_t x, uint8_t y);
  void onButton(Button button, Callback callback);
  void onButtonUp();
  void onButtonDown();
  void onButtonMenu();
};

#endif