#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include "screen.h"
#include <FastLED.h>

#define WIDTH 16
#define HEIGHT 16
#define MAX_SNAKE_LENGTH 256

struct Point {
  uint8_t x, y;
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

class SnakeGame : public Screen {
  private:
    CRGB leds[WIDTH * HEIGHT];
    Point snake[MAX_SNAKE_LENGTH];
    uint16_t length;
    int8_t dx, dy;
    uint32_t lastMoveTime;
    uint16_t moveInterval; // milliseconds between moves
    Point food;

    void moveSnake();
    void generateFood();
    bool isOnSnake(Point p);
    uint16_t getIndex(uint8_t x, uint8_t y);

  public:
    SnakeGame();
    CRGB* draw() override;
    void onButton(Button button, Callback callback = NULL) override;
    void onButtonUp() override;
    void onButtonDown() override;
    void onButtonLeft() override;
    void onButtonRight() override;
    uint8_t getFrame() override;
};

#endif
