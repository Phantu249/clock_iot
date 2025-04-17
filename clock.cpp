#include "clock.h"
#include <FastLED.h>
#include <stdlib.h>

// Font 3x5
const uint8_t digit_patterns[10][5] = {
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

Clock::Clock(uint32_t timestamp, int8_t offset) {
  _timestamp = timestamp;
  _offset = offset;
  _frame = 1;
}

CRGB* Clock::draw() {
  CRGB buffer[NUM_LEDS];

  fill_solid(buffer, NUM_LEDS, CRGB::Black);
  fill_solid(frame, NUM_LEDS, CRGB::Black);

  uint8_t hours = ((_timestamp + (_offset * 3600)) % 86400) / 3600;
  uint8_t minutes = (_timestamp % 3600) / 60;

  uint8_t h1 = hours / 10;
  uint8_t h2 = hours % 10;
  uint8_t m1 = minutes / 10;
  uint8_t m2 = minutes % 10;

  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 3; x++) {
      if (digit_patterns[h1][y] & (1 << (2-x))) {
        buffer[(y + 9) * 16 + x + 4] = CRGB::Red;
      }
      
      if (digit_patterns[h2][y] & (1 << (2-x))) {
        buffer[(y + 9) * 16 + x + 9] = CRGB::Red;
      }

      if (digit_patterns[m1][y] & (1 << (2-x))) {
        buffer[(y + 2) * 16 + x + 4] = CRGB::Red;
      }

      if (digit_patterns[m2][y] & (1 << (2-x))) {
        buffer[(y + 2) * 16 + x + 9] = CRGB::Red;
      }
    }
  }

  for (int y = 0; y < NUM_COLS; y += 1) {
    for (int x = 0; x < NUM_ROWS; x += 1) {
      int srcIndex = y * NUM_COLS + x;
      int zigzagX = (y % 2 == 0) ? x : 15 - x;
      int dstIndex = y * NUM_COLS + zigzagX;
      frame[dstIndex] = buffer[srcIndex];
    }
  }

  return frame;
}


void Clock::setTimestamp(uint32_t timestamp) {
  _timestamp = timestamp;
}
