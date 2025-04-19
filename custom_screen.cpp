#include "custom_screen.h"

CustomScreen::CustomScreen() {
  isGif = false;
  _frame = 3;
  totalFrame = 0;
  currentFrame = 0;

  preferences.begin("custom_screen", true);
  if (!preferences.isKey("total_frame")) {
    Serial.println("Failed to find total_frame key");
    preferences.end();
    return;
  }

  totalFrame = preferences.getUChar("total_frame", 0);
  if (totalFrame < 1 || totalFrame > MAX_FRAME) {
    Serial.printf("Invalid total_frame: %d (must be 1 to %d)\n", totalFrame, MAX_FRAME);
    totalFrame = 0;
    preferences.end();
    return;
  }
  Serial.printf("Retrieved total_frame: %d\n", totalFrame);

  for (int i = 0; i < totalFrame; i++) {
    char frameName[16];
    snprintf(frameName, sizeof(frameName), "frame_%d", i);
    if (preferences.isKey(frameName)) {
      size_t len = preferences.getBytesLength(frameName);
      if (len != NUM_LEDS) {
        Serial.printf("Frame %s size mismatch: expected %d bytes, got %d\n", frameName, NUM_LEDS, len);
        totalFrame = i;  // Chỉ sử dụng các frame hợp lệ trước đó
        break;
      }
      preferences.getBytes(frameName, frames[i], NUM_LEDS);
      Serial.printf("Retrieved frame: %s (%d bytes)\n", frameName, len);
    } else {
      Serial.printf("Failed to find frame: %s\n", frameName);
      totalFrame = i;  // Chỉ sử dụngずに các frame hợp lệ trước đó
      break;
    }
  }

  preferences.end();
  if (totalFrame == 0) {
    Serial.println("No valid frames loaded");
  }
}

int CustomScreen::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);
  }
}

void CustomScreen::renderFrame() {
  if (totalFrame < 1 || currentFrame >= totalFrame) {
    Serial.println("Invalid frame data");
    return;
  }

  for (int j = 0; j < NUM_LEDS; j++) {
    frame[j] = CRGB::Black;
  }

  for (int y = 0; y < NUM_ROWS; y++) {
    for (int x = 0; x < NUM_COLS; x++) {
      int index = getIndex(x, y);
      int pixelIndex = y * NUM_COLS + x;
      if (pixelIndex < NUM_LEDS && index < NUM_LEDS) {
        uint8_t colorIndex = frames[currentFrame][pixelIndex];
        if (colorIndex < 16) {  // Giả sử mảng colors có 16 phần tử
          frame[index] = colors[colorIndex];
        }
      }
    }
  }
}

CRGB* CustomScreen::draw() {
  if (totalFrame < 1) {
    static uint8_t hue = 0;
    hue += 3;
    fill_rainbow(frame, NUM_LEDS, hue, 5);
    return frame;
  }
  renderFrame();
  if (isGif) {
    currentFrame = (currentFrame + 1) % totalFrame;
  }
  return frame;
}

void CustomScreen::onButton(Button button, Callback callback) {
  switch (button) {
    case BUTTON_UP:
      onButtonUp();
      break;
    case BUTTON_DOWN:
      onButtonDown();
      break;
    case BUTTON_MENU:
      onButtonMenu();
      break;
    default:
      return;
  }
};

void CustomScreen::onButtonUp() {
  if (!totalFrame) return;
  isGif = false;
  currentFrame = (currentFrame + 1) % totalFrame;
};
void CustomScreen::onButtonDown() {
  if (!totalFrame) return;
  isGif = false;
  currentFrame = (currentFrame - 1 + totalFrame) % totalFrame;
};
void CustomScreen::onButtonMenu() {
  if (!isGif) isGif = true;
};