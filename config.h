#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

#define LED_PIN 17  // Chân tín hiệu
#define NUM_ROWS 16
#define NUM_COLS 16
#define NUM_LEDS NUM_ROWS* NUM_COLS  // Số lượng LED
#define LED_TYPE NEOPIXEL            // Loại LED
#define BRIGHTNESS 5

#ifndef MAX_FRAME
#define MAX_FRAME 5
#endif

typedef enum : uint8_t {
  MANUAL,
  AUTO
} TimeMode;

typedef enum : uint8_t {
  CLOCK,
  MENU,
  GAME,
  BLE,
  CUSTOM
} State;

const CRGB colors[16] = {
  CRGB::Black,    // OFF (0)
  CRGB::Red,      // RED (1)
  CRGB::Green,    // GREEN (2)
  CRGB::Blue,     // BLUE (3)
  CRGB::Yellow,   // YELLOW (4)
  CRGB::Cyan,     // CYAN (5)
  CRGB::Magenta,  // MAGENTA (6)
  CRGB::White,    // WHITE (7)
  CRGB::Orange,   // ORANGE (8)
  CRGB::Purple,   // PURPLE (9)
  CRGB::Pink,     // PINK (10)
  CRGB::Lime,     // LIME (11)
  CRGB::Teal,     // TEAL (12)
  CRGB::Violet,   // VIOLET (13)
  CRGB::Gold,     // GOLD (14)
  CRGB::Silver    // SILVER (15)
};


struct Alarm {
  int8_t hour;
  int8_t minute;
};

// Cấu trúc lưu thông tin debounce cho mỗi nút
struct ButtonInfo {
  int pin;
  volatile unsigned long lastDebounceTime;
};

const char DELIMITER = ',';
#endif