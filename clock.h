#ifndef CLOCK_H
#define CLOCK_H

#include "screen.h"
#include "FastLED.h"
#include <cstdint>

class Clock : public Screen {
  protected:
    uint32_t _timestamp;

  public:
    Clock(uint32_t timestamp);

    CRGB* draw() override;
    void setTimestamp(uint32_t timestamp);
};

#endif
