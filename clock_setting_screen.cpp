#include "clock_setting_screen.h"
#include "menu.h"

ClockSettingScreen::ClockSettingScreen(uint32_t& timestamp, int8_t offset) : timestamp(timestamp), offset(offset) {
  _frame = 10; // Match typical frame rate for smooth display
  settingState = SETTING_HOUR;
  blinkState = true;
  lastBlinkTime = millis();
  // Initialize timeinfo with timestamp + offset (local time)
  time_t adjustedTime = static_cast<time_t>(timestamp) + (offset * 3600);
  localtime_r(&adjustedTime, &timeinfo);
}

uint16_t ClockSettingScreen::getLedIndex(uint8_t x, uint8_t y) {
  // Flip y to start from bottom
  y = MATRIX_HEIGHT - 1 - y;
  // If row is odd, reverse x direction
  if (y % 2 == 1) {
    x = MATRIX_WIDTH - 1 - x;
  }
  return y * MATRIX_WIDTH + x;
}

void ClockSettingScreen::drawDigit(uint8_t digit, uint8_t x, uint8_t y, CRGB color) {
  // Simple 3x5 digit patterns (0-9)
  static const uint8_t digits[10][15] = {
    {0x07, 0x05, 0x05, 0x05, 0x07}, // 0
    {0x02, 0x02, 0x02, 0x02, 0x02}, // 1
    {0x07, 0x04, 0x07, 0x01, 0x07}, // 2
    {0x07, 0x01, 0x07, 0x01, 0x07}, // 3
    {0x01, 0x01, 0x07, 0x05, 0x05}, // 4
    {0x07, 0x01, 0x07, 0x04, 0x07}, // 5
    {0x07, 0x05, 0x07, 0x04, 0x07}, // 6
    {0x01, 0x01, 0x01, 0x01, 0x07}, // 7
    {0x07, 0x05, 0x07, 0x05, 0x07}, // 8
    {0x07, 0x01, 0x07, 0x05, 0x07}, // 9
  };

  for (uint8_t dy = 0; dy < 5; dy++) {
    for (uint8_t dx = 0; dx < 3; dx++) {
      if (digits[digit][dy * 3 + dx]) {
        frame[getLedIndex(x + dx, y + dy)] = color;
      }
    }
  }
}

void ClockSettingScreen::drawTime() {
  // Clear frame
  fill_solid(frame, NUM_LEDS, CRGB::Black);

  // Get hours and minutes
  uint8_t hours = timeinfo.tm_hour;
  uint8_t minutes = timeinfo.tm_min;

  // Convert to digits
  uint8_t h1 = hours / 10;
  uint8_t h2 = hours % 10;
  uint8_t m1 = minutes / 10;
  uint8_t m2 = minutes % 10;

  // Define colors
  CRGB hourColor = (settingState == SETTING_HOUR && !blinkState) ? CRGB::Black : CRGB::White;
  CRGB minuteColor = (settingState == SETTING_MINUTE && !blinkState) ? CRGB::Black : CRGB::White;

  // Giờ (ở hàng trên – ví dụ y = 10)
  drawDigit(h1, 4, 2, hourColor);  // chỉnh lại x nếu cần căn giữa
  drawDigit(h2, 8, 2, hourColor);

  // Phút (ở hàng dưới – ví dụ y = 3)
  drawDigit(m1, 4, 10, minuteColor);
  drawDigit(m2, 8, 10, minuteColor);
}

CRGB* ClockSettingScreen::draw() {
  if (settingState != SETTING_DONE) {
    if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
      blinkState = !blinkState;
      lastBlinkTime = millis();
    }
  }

  drawTime();  // Luôn vẽ lại frame mới nhất
  return frame;
}

void ClockSettingScreen::onButton(Button button, Callback callback) {
  switch (button) {
    case BUTTON_UP:
      onButtonUp();
      break;
    case BUTTON_DOWN:
      onButtonDown();
      break;
    case BUTTON_LEFT:
      onButtonLeft();
      break;
    case BUTTON_RIGHT:
      onButtonRight();
      break;
    case BUTTON_BACK:
      {
        Screen* s = new Menu();
        callback(s, State::MENU);
      }
      break;
    case BUTTON_MENU:
      if (settingState == SETTING_HOUR) {
        settingState = SETTING_MINUTE;
      } else if (settingState == SETTING_MINUTE) {
        // Update global timestamp: convert timeinfo back to UTC timestamp
        time_t newTime = mktime(&timeinfo);
        timestamp = static_cast<uint32_t>(newTime - (offset * 3600));
        settingState = SETTING_DONE;
        Screen* s = new Menu();
        callback(s, State::MENU);
      }
      break;
  }
}

void ClockSettingScreen::onButtonUp() {
  if (settingState == SETTING_HOUR) {
    timeinfo.tm_hour = (timeinfo.tm_hour + 1) % 24;
  } else if (settingState == SETTING_MINUTE) {
    timeinfo.tm_min = (timeinfo.tm_min + 1) % 60;
  }
}

void ClockSettingScreen::onButtonDown() {
  if (settingState == SETTING_HOUR) {
    timeinfo.tm_hour = (timeinfo.tm_hour - 1 + 24) % 24;
  } else if (settingState == SETTING_MINUTE) {
    timeinfo.tm_min = (timeinfo.tm_min - 1 + 60) % 60;
  }
}

void ClockSettingScreen::onButtonLeft() {
  if (settingState == SETTING_HOUR) {
    timeinfo.tm_hour = (timeinfo.tm_hour - 1 + 24) % 24;
  } else if (settingState == SETTING_MINUTE) {
    timeinfo.tm_min = (timeinfo.tm_min - 1 + 60) % 60;
  }
}

void ClockSettingScreen::onButtonRight() {
  if (settingState == SETTING_HOUR) {
    timeinfo.tm_hour = (timeinfo.tm_hour + 1) % 24;
  } else if (settingState == SETTING_MINUTE) {
    timeinfo.tm_min = (timeinfo.tm_min + 1) % 60;
  }
}

void ClockSettingScreen::onButtonBack() {
  // Handled in onButton with callback
}

void ClockSettingScreen::onButtonMenu() {
  // Handled in onButton with callback
}