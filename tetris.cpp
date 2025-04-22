#include "tetris.h"
#include "menu.h"  // Assuming a Menu class exists as in snake game

const PointQ TetrisGame::tetrominoes[NUM_TETROMINOES][4][4] = {
  // I
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 } }, { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 } } },
  // O
  { { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }, { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }, { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }, { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } } },
  // T
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 1, 1 } }, { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 0, 1 } }, { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 1, 0 } }, { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 1 } } },
  // S
  { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } }, { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 2 } }, { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } }, { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 2 } } },
  // Z
  { { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 2, 0 } }, { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 } }, { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 2, 0 } }, { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 2 } } },
  // J
  { { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } }, { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 0, 2 } }, { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 } }, { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 0 } } },
  // L
  { { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 0 } }, { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 2 } }, { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 2, 0 } }, { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 } } }
};

const CRGB TetrisGame::colors[8] = {
  CRGB::Black,   // 0: Empty
  CRGB::Cyan,    // 1: I
  CRGB::Yellow,  // 2: O
  CRGB::Purple,  // 3: T
  CRGB::Green,   // 4: S
  CRGB::Red,     // 5: Z
  CRGB::Blue,    // 6: J
  CRGB::Orange   // 7: L
};

TetrisGame::TetrisGame() {
  memset(playfield, 0, sizeof(playfield));
  state = PLAYING;
  fallInterval = 500;  // Initial fall speed in milliseconds
  lastFallTime = millis();
  score = 0;
  _frame = 15;  // Frame rate similar to SnakeGame
  spawnBlock();
}

void TetrisGame::spawnBlock() {
  currentType = random(0, NUM_TETROMINOES);
  currentX = 3;  // Center spawn position for 10-wide playfield
  currentY = 0;
  currentRotation = 0;
  if (!isValidPosition(currentX, currentY, currentRotation)) {
    state = GAME_OVER;
  }
}

bool TetrisGame::isValidPosition(int8_t x, int8_t y, uint8_t rotation) {
  for (int i = 0; i < 4; ++i) {
    int8_t abs_x = x + tetrominoes[currentType][rotation][i].x;
    int8_t abs_y = y + tetrominoes[currentType][rotation][i].y;
    if (abs_x < 0 || abs_x >= PLAYFIELD_WIDTH || abs_y >= PLAYFIELD_HEIGHT) {
      return false;
    }
    if (abs_y >= 0 && playfield[abs_y][abs_x] != 0) {
      return false;
    }
  }
  return true;
}

bool TetrisGame::tryMove(int8_t dx, int8_t dy) {
  int8_t newX = currentX + dx;
  int8_t newY = currentY + dy;
  if (isValidPosition(newX, newY, currentRotation)) {
    currentX = newX;
    currentY = newY;
    return true;
  }
  return false;
}

void TetrisGame::landBlock() {
  for (int i = 0; i < 4; ++i) {
    int8_t abs_x = currentX + tetrominoes[currentType][currentRotation][i].x;
    int8_t abs_y = currentY + tetrominoes[currentType][currentRotation][i].y;
    if (abs_y >= 0) {
      playfield[abs_y][abs_x] = currentType + 1;  // Store 1-7 for colors
    }
  }
}

void TetrisGame::checkLines() {
  int linesCleared = 0;
  for (int y = PLAYFIELD_HEIGHT - 1; y >= 0; --y) {
    bool full = true;
    for (int x = 0; x < PLAYFIELD_WIDTH; ++x) {
      if (playfield[y][x] == 0) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int yy = y; yy > 0; --yy) {
        memcpy(playfield[yy], playfield[yy - 1], PLAYFIELD_WIDTH);
      }
      memset(playfield[0], 0, PLAYFIELD_WIDTH);
      linesCleared++;
      y++;  // Re-check the same row after shift
    }
  }
  if (linesCleared > 0) {
    score += linesCleared * 100;  // Simple scoring
  }
}

CRGB *TetrisGame::draw() {
  fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);

  // Draw left border
  if (PLAYFIELD_LEFT - 1 >= 0) {
    for (int y = 0; y < HEIGHT; ++y) {
      leds[getIndex(PLAYFIELD_LEFT - 1, y)] = CRGB::Gray;
    }
  }

  // Draw right border
  if (PLAYFIELD_LEFT + PLAYFIELD_WIDTH < WIDTH) {
    for (int y = 0; y < HEIGHT; ++y) {
      leds[getIndex(PLAYFIELD_LEFT + PLAYFIELD_WIDTH, y)] = CRGB::Gray;
    }
  }

  if (state == PLAYING) {
    unsigned long now = millis();
    if (now - lastFallTime >= fallInterval) {
      lastFallTime = now;
      if (!tryMove(0, 1)) {  // Try to fall
        landBlock();
        checkLines();
        spawnBlock();
      }
    }

    // Render playfield
    for (int y = 0; y < PLAYFIELD_HEIGHT; ++y) {
      for (int x = 0; x < PLAYFIELD_WIDTH; ++x) {
        if (playfield[y][x] != 0) {
          leds[getIndex(x + PLAYFIELD_LEFT, y)] = colors[playfield[y][x]];
        }
      }
    }

    // Render falling block
    for (int i = 0; i < 4; ++i) {
      int8_t abs_x = currentX + tetrominoes[currentType][currentRotation][i].x;
      int8_t abs_y = currentY + tetrominoes[currentType][currentRotation][i].y;
      if (abs_y >= 0 && abs_x >= 0 && abs_x < PLAYFIELD_WIDTH) {
        leds[getIndex(abs_x + PLAYFIELD_LEFT, abs_y)] = colors[currentType + 1];
      }
    }
  } else {
    drawMenu();
  }

  return leds;
}

void TetrisGame::drawMenu() {
  // Simple menu similar to SnakeGame
  fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);

  // Draw "X" (resume/restart) at top-left
  const uint8_t xBitmap[5][5] = {
    { 1, 0, 0, 0, 1 },
    { 0, 1, 0, 1, 0 },
    { 0, 0, 1, 0, 0 },
    { 0, 1, 0, 1, 0 },
    { 1, 0, 0, 0, 1 }
  };

  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 5; x++) {
      if (xBitmap[y][x]) {
        leds[getIndex(x + 1, HEIGHT - 10 + y)] = CRGB::White;
      }
    }
  }

  // Draw "Check" (exit) at top-right
  const uint8_t checkBitmap[6][6] = {
    { 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 1, 0, 0 },
    { 1, 0, 1, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0 }
  };

  for (uint8_t y = 0; y < 6; y++) {
    for (uint8_t x = 0; x < 6; x++) {
      if (checkBitmap[y][x]) {
        leds[getIndex(x + WIDTH - 7, HEIGHT - 10 + y)] = CRGB::White;
      }
    }
  }
}

uint16_t TetrisGame::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = HEIGHT - 1 - y;  // Invert y-coordinate to flip the display
  if (physicalY % 2 == 0) {
    return physicalY * WIDTH + x;
  } else {
    return physicalY * WIDTH + (WIDTH - 1 - x);
  }
}

void TetrisGame::onButton(Button button, Callback callback) {
  if (state == PAUSED) {
    if (button == BUTTON_BACK) {
      state = PLAYING;
    } else if (button == BUTTON_MENU) {
      if (callback) {
        Screen *s = new Menu();
        callback(s, State::MENU);
      }
    }
  } else if (state == GAME_OVER) {
    if (button == BUTTON_BACK) {
      memset(playfield, 0, sizeof(playfield));
      score = 0;
      spawnBlock();
      state = PLAYING;
    } else if (button == BUTTON_MENU) {
      if (callback) {
        Screen *s = new Menu();
        callback(s, State::MENU);
      }
    }
  } else if (state == PLAYING) {
    switch (button) {
      case BUTTON_LEFT: onButtonLeft(); break;
      case BUTTON_RIGHT: onButtonRight(); break;
      case BUTTON_DOWN: onButtonDown(); break;
      case BUTTON_UP: onButtonUp(); break;
      case BUTTON_BACK: state = PAUSED; break;
      case BUTTON_MENU: /* Reserved */ break;
    }
  }
}

void TetrisGame::onButtonLeft() {
  tryMove(-1, 0);
}

void TetrisGame::onButtonRight() {
  tryMove(1, 0);
}

void TetrisGame::onButtonDown() {
  tryMove(0, 1);
}

void TetrisGame::onButtonUp() {
  uint8_t newRotation = (currentRotation + 1) % 4;
  if (isValidPosition(currentX, currentY, newRotation)) {
    currentRotation = newRotation;
  }
}

void TetrisGame::onButtonBack() {}

void TetrisGame::onButtonMenu() {}

uint8_t TetrisGame::getFrame() {
  return _frame;
}
