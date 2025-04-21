#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "screen.h"
#include <FastLED.h>

#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 16
#define PLAYFIELD_LEFT 3
#define PLAYFIELD_TOP 0
#define NUM_TETROMINOES 7
#define WIDTH 16
#define HEIGHT 16

struct PointQ {
  int8_t x, y;
};

class TetrisGame : public Screen {
private:
  CRGB leds[WIDTH * HEIGHT];
  uint8_t playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
  uint8_t currentType;
  int8_t currentX, currentY;
  uint8_t currentRotation;
  uint32_t lastFallTime;
  uint16_t fallInterval;
  uint32_t score;
  enum GameState { PLAYING, PAUSED, GAME_OVER };
  GameState state;

  static const PointQ tetrominoes[NUM_TETROMINOES][4][4];
  static const CRGB colors[8];

  void spawnBlock();
  bool tryMove(int8_t dx, int8_t dy);
  bool isValidPosition(int8_t x, int8_t y, uint8_t rotation);
  void landBlock();
  void checkLines();
  void drawMenu();
  uint16_t getIndex(uint8_t x, uint8_t y);

public:
  TetrisGame();
  CRGB* draw() override;
  void onButton(Button button, Callback callback = NULL) override;
  void onButtonUp() override;
  void onButtonDown() override;
  void onButtonLeft() override;
  void onButtonRight() override;
  void onButtonBack() override;
  void onButtonMenu() override;
  uint8_t getFrame() override;
};

#endif
