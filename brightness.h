#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include "screen.h"
#include "config.h"
#include <Preferences.h>
#include <FastLED.h>

#ifndef MAX_BRIGHTNESS
#define MAX_BRIGHTNESS 256
#endif

#ifndef NUM_BRIGHTNESS_PIXEL
#define NUM_BRIGHTNESS_PIXEL 12
#endif

class Brightness : public Screen {
protected:
  uint8_t currentBrightness;
  Preferences preferences;

public:
  Brightness();
  CRGB* draw() override;
  void renderFrame();
  int getIndex(uint8_t x, uint8_t y);
  void onButton(Button button, Callback callback);
  void onButtonLeft();
  void onButtonRight();
  void onButtonMenu();
  void updateBrightnessInFrame();
};

#endif