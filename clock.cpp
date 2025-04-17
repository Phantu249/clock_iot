#include "clock.h"
#include <FastLED.h>
#include <stdlib.h>

// Font 3x5
const uint8_t digit_patterns[10][5] = {
    {0x07, 0x05, 0x05, 0x05, 0x07},
    {0x02, 0x02, 0x02, 0x02, 0x02},
    {0x07, 0x01, 0x07, 0x04, 0x07},
    {0x07, 0x01, 0x07, 0x01, 0x07},
    {0x05, 0x05, 0x07, 0x01, 0x01},
    {0x07, 0x04, 0x07, 0x01, 0x07},
    {0x07, 0x04, 0x07, 0x05, 0x07},
    {0x07, 0x01, 0x01, 0x01, 0x01},
    {0x07, 0x05, 0x07, 0x05, 0x07},
    {0x07, 0x05, 0x07, 0x01, 0x07} 
};

Clock::Clock(uint32_t timestamp) {
  _timestamp = timestamp;
  _frame = 1;
}

CRGB* Clock::draw() {
  fill_solid(frame, NUM_LEDS, CRGB::Black); 

  uint8_t hours = (_timestamp % 86400) / 3600;
  uint8_t minutes = (_timestamp % 3600) / 60;

  uint8_t h1 = hours / 10;
  uint8_t h2 = hours % 10;
  uint8_t m1 = minutes / 10;
  uint8_t m2 = minutes % 10;

  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 3; x++) {
      if (digit_patterns[h1][y] & (1 << (2-x))) {
        frame[(y + 2) * 16 + x + 4] = CRGB::Red;
      }
      
      if (digit_patterns[h2][y] & (1 << (2-x))) {
        frame[(y + 2) * 16 + x + 9] = CRGB::Red;
      }

      if (digit_patterns[m1][y] & (1 << (2-x))) {
        frame[(y + 9) * 16 + x + 4] = CRGB::Green;
      }

      if (digit_patterns[m2][y] & (1 << (2-x))) {
        frame[(y + 9) * 16 + x + 9] = CRGB::Green;
      }
    }
  }

  return frame;
}

void Clock::setTimestamp(uint32_t timestamp) {
  _timestamp = timestamp;
}
