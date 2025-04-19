#ifndef CLOCK_H
#define CLOCK_H

#include "screen.h"
#include "FastLED.h"
#include <cstdint>

class Clock : public Screen {
  protected:
    uint32_t _timestamp;
    int8_t _offset;

  public:
    Clock(uint32_t timestamp, int8_t offset);

    CRGB* draw() override;
    void setTimestamp(uint32_t timestamp);
};

#endif