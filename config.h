#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>

#define LED_PIN     5         // Chân tín hiệu
#define NUM_ROWS     16
#define NUM_COLS     16
#define NUM_LEDS     NUM_ROWS * NUM_COLS     // Số lượng LED
#define LED_TYPE    NEOPIXEL   // Loại LED 
#define BRIGHTNESS 5

typedef enum : uint8_t {
  CLOCK,
  MENU,
  GAME
} State;

#endif