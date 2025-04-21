#include "brightness.h"

uint8_t brightnessAdjustFrame[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 
  0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
  0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 
  0, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 0, 
  0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Brightness::Brightness() {
  _frame = 3;
  preferences.begin("app", true);
  currentBrightness = preferences.getUChar("brightness", 10);
  preferences.end();
  updateBrightnessInFrame();
}

int Brightness::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);
  }
}

void Brightness::renderFrame() {
  for (int j = 0; j < NUM_LEDS; j++) {
    frame[j] = CRGB::Black;
  }

  for (int y = 0; y < NUM_ROWS; y++) {
    for (int x = 0; x < NUM_COLS; x++) {
      int index = getIndex(x, y);
      int pixelIndex = y * NUM_COLS + x;
      if (pixelIndex < NUM_LEDS && index < NUM_LEDS) {
        uint8_t colorIndex = brightnessAdjustFrame[pixelIndex];
        if (colorIndex < 16) {  // Giả sử mảng colors có 16 phần tử
          frame[index] = colors[colorIndex];
        }
      }
    }
  }
}

CRGB* Brightness::draw() {
  renderFrame();
  return frame;
}

void Brightness::updateBrightnessInFrame() {
  int numBrightness = (currentBrightness * NUM_BRIGHTNESS_PIXEL) / MAX_BRIGHTNESS;

  // Cập nhật mảng brightnessAdjustFrame
  for (int i = 0; i < NUM_BRIGHTNESS_PIXEL; i++) {
    if (i < numBrightness) {
      brightnessAdjustFrame[i + 114] = CRGB::White;
    } else {
      brightnessAdjustFrame[i + 114] = CRGB::Black;
    }
  }
}



void Brightness::onButton(Button button, Callback callback) {
  switch (button) {
    case BUTTON_LEFT:
      onButtonLeft();
      break;
    case BUTTON_RIGHT:
      onButtonRight();
      break;
    case BUTTON_MENU:
      onButtonMenu();
      break;
    default:
      return;
  }
};

void Brightness::onButtonRight() {
  currentBrightness = (currentBrightness + MAX_BRIGHTNESS / NUM_BRIGHTNESS_PIXEL) % MAX_BRIGHTNESS;
  updateBrightnessInFrame();  // Cập nhật mảng
};
void Brightness::onButtonLeft() {
  currentBrightness = (currentBrightness - MAX_BRIGHTNESS / NUM_BRIGHTNESS_PIXEL + MAX_BRIGHTNESS) % MAX_BRIGHTNESS;
  updateBrightnessInFrame();  // Cập nhật mảng
};
void Brightness::onButtonMenu() {
  FastLED.setBrightness(currentBrightness);
  preferences.begin("app", false);
  currentBrightness = preferences.putUChar("brightness", currentBrightness);
  preferences.end();
};