#include "custom_screen.h"

CustomScreen::CustomScreen() {
  _frame = 3;
  preferences.begin("custom_screen", true);
  if (!preferences.isKey("total_frame")) {
    Serial.println("False to get total_frame");
    return;
  }
  totalFrame = preferences.getUChar("total_frame");
  for (int i = 0; i < totalFrame; i++) {
    String frameName = "frame_" + i;
    if (preferences.isKey(frameName.c_str())) {
        preferences.getBytes(frameName.c_str(), frames[i], NUM_LEDS);
    } else {
      Serial.printf("False to get frame %s \n", frameName);
      return;
    }
  }
  preferences.end();
}

// getIndex func for convert x,y index from 2D array to 1D CRGB array use for FastLED
int CustomScreen::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);
  }
}

void CustomScreen::renderFrame() {

  for (int j = 0; j < NUM_LEDS; j++) {
    frame[j] = CRGB::Black;
  }

  for (int y = 0; y < NUM_ROWS; y++) {
    for (int x = 0; x < NUM_COLS; x++) {
      int index = getIndex(x, y);
      int pixelIndex = y * NUM_COLS + x;                           // 1 byte mỗi pixel
      uint8_t colorIndex = frames[currentFrame][pixelIndex]);  // Lấy giá trị 0-15
      frame[index] = colors[colorIndex];
    }
  }

  CRGB* CustomScreen::draw() {
    renderFrame();
    currentFrame = (currentFrame + 1) % totalFrame;
    return frame;
  }
}
