#include "snake.h"
#include "menu.h"

SnakeGame::SnakeGame() {
  length = 3;
  snake[0] = {5, 5};
  snake[1] = {4, 5};
  snake[2] = {3, 5};
  dx = 1;
  dy = 0;
  _frame = 30;
  moveInterval = 200;
  lastMoveTime = millis();
  gameOver = false;
  inMenu = false;
  generateFood();
}

uint16_t SnakeGame::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = y;
  if (physicalY % 2 == 0) {
    return physicalY * WIDTH + x;
  } else {
    return physicalY * WIDTH + (WIDTH - 1 - x);
  }
}

bool SnakeGame::isOnSnake(Point p) {
  for (int i = 0; i < length; ++i) {
    if (snake[i] == p) return true;
  }
  return false;
}

void SnakeGame::generateFood() {
  Point p;
  do {
    p.x = random(0, WIDTH);
    p.y = random(0, HEIGHT);
  } while (isOnSnake(p));
  food = p;
}

void SnakeGame::moveSnake() {
  Point newHead = {
    (snake[0].x + dx + WIDTH) % WIDTH,
    (snake[0].y + dy + HEIGHT) % HEIGHT
  };

  bool ate = (newHead == food);
  if (ate) {
    if (length < MAX_SNAKE_LENGTH)
      length++;
    generateFood();
  }

  for (int i = length - 1; i > 0; --i) {
    snake[i] = snake[i - 1];
  }

  snake[0] = newHead;

  for (int i = 1; i < length; ++i) {
    if (newHead == snake[i]) {
      gameOver = true;
      inMenu = true;  // Vào menu khi game over
      return;
    }
  }
}

CRGB* SnakeGame::draw() {
  unsigned long now = millis();

  if (inMenu) {
    fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);
    drawMenu();  // Hiển thị menu với nút X và V
    return leds;
  }

  if (now - lastMoveTime >= moveInterval) {
    lastMoveTime = now;
    moveSnake();
  }

  fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);
  for (int i = 0; i < length; ++i) {
    if (i == 0)
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Red;
    else
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Green;
  }

  leds[getIndex(food.x, food.y)] = CRGB::Yellow;

  return leds;
}

void SnakeGame::onButton(Button button, Callback callback) {
  if (inMenu) {
    switch (button) {
      case BUTTON_BACK:  // Tương ứng nút "X"
        if (gameOver) {
          resetGame();  // Chơi lại từ đầu
        } else {
          restoreState();  // Khôi phục trò chơi
        }
        inMenu = false;
        break;
      case BUTTON_MENU:  // Tương ứng nút "V"
        if (callback) {
          Screen *s = new Menu();
          callback(s, State::MENU);  // Thoát về Clock
        }
        break;
      default:
        break;
    }
    return;
  }

  switch (button) {
    case BUTTON_UP:    onButtonUp(); break;
    case BUTTON_DOWN:  onButtonDown(); break;
    case BUTTON_LEFT:  onButtonLeft(); break;
    case BUTTON_RIGHT: onButtonRight(); break;
    case BUTTON_BACK:
      saveState();  // Lưu trạng thái
      inMenu = true;  // Vào menu
      break;
    case BUTTON_MENU:
      // Không làm gì ngoài menu, hoặc có thể tùy chỉnh
      break;
    default: break;
  }
}

void SnakeGame::onButtonUp() {
  if (dy == 0) { dx = 0; dy = 1; }
}

void SnakeGame::onButtonDown() {
  if (dy == 0) { dx = 0; dy = -1; }
}

void SnakeGame::onButtonLeft() {
  if (dx == 0) { dx = -1; dy = 0; }
}

void SnakeGame::onButtonRight() {
  if (dx == 0) { dx = 1; dy = 0; }
}

void SnakeGame::onButtonBack() {
  // Xử lý trong onButton()
}

void SnakeGame::onButtonMenu() {
  // Xử lý trong onButton()
}

uint8_t SnakeGame::getFrame() {
  return _frame;
}

bool SnakeGame::isGameOver() {
  return gameOver;
}

void SnakeGame::endGame() {
  gameOver = true;
  inMenu = true;
}

void SnakeGame::saveState() {
  savedLength = length;
  for (int i = 0; i < length; ++i) {
    savedSnake[i] = snake[i];
  }
  savedDx = dx;
  savedDy = dy;
  savedFood = food;
  savedLastMoveTime = lastMoveTime;
  savedGameOver = gameOver;
}

void SnakeGame::restoreState() {
  length = savedLength;
  for (int i = 0; i < length; ++i) {
    snake[i] = savedSnake[i];
  }
  dx = savedDx;
  dy = savedDy;
  food = savedFood;
  lastMoveTime = savedLastMoveTime;
  gameOver = savedGameOver;
}

void SnakeGame::resetGame() {
  length = 3;
  snake[0] = {5, 5};
  snake[1] = {4, 5};
  snake[2] = {3, 5};
  dx = 1;
  dy = 0;
  lastMoveTime = millis();
  gameOver = false;
  inMenu = false;
  generateFood();
}

void SnakeGame::drawMenu() {
  // Vẽ nút "X" (5x5) ở góc trái trên (x=1, y=HEIGHT-6)
  const uint8_t xBitmap[5][5] = {
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1}
  };
  for (uint8_t y = 0; y < 5; y++) {
    for (uint8_t x = 0; x < 5; x++) {
      if (xBitmap[y][x] && x + 1 < WIDTH && y + HEIGHT - 6 < HEIGHT) {
        leds[getIndex(x + 1, HEIGHT - 6 + y)] = CRGB::White;
      }
    }
  }

  // Vẽ nút "Check" (5x5) ở góc phải trên (x=WIDTH-6, y=HEIGHT-6)
  const uint8_t checkBitmap[6][6] = {
    {0, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1}
  };
  for (uint8_t y = 0; y < 6; y++) {
    for (uint8_t x = 0; x < 6; x++) {
      if (checkBitmap[y][x] && x + WIDTH - 6 < WIDTH && y + HEIGHT - 6 < HEIGHT) {
        leds[getIndex(x + WIDTH - 7, HEIGHT - 6 + y)] = CRGB::White;
      }
    }
  }

  // Vẽ chữ "Exit" (4 ký tự, mỗi ký tự 3x3) ở giữa dưới (x=4, y=8)
  const uint8_t eBitmap[3][3] = {
    {1, 1, 1},
    {1, 1, 0},
    {1, 1, 1}
  };
  const uint8_t xBitmapSmall[3][3] = {
    {1, 0, 1},
    {0, 1, 0},
    {1, 0, 1}
  };
  const uint8_t iBitmap[3][3] = {
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0}
  };
  const uint8_t tBitmap[3][3] = {
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0}
  };

  // Mảng con trỏ đến các bitmap, sắp xếp để hiển thị "E X I T" từ trái sang phải
  const uint8_t (*exitBitmaps[4])[3] = {eBitmap, xBitmapSmall, iBitmap, tBitmap};
  // Offset cho từng ký tự: E(0), X(4), I(8), T(9)
  const uint8_t xOffsets[4] = {0, 4, 7, 10};
  for (int charIdx = 0; charIdx < 4; charIdx++) {
    for (uint8_t y = 0; y < 3; y++) {
      for (uint8_t x = 0; x < 3; x++) {
        // Chỉ vẽ cột giữa cho "I" (charIdx=2, x=1)
        if ((charIdx != 2 || x == 1) && exitBitmaps[charIdx][2 - y][x] && x + 2 + xOffsets[charIdx] < WIDTH && y + 6 < HEIGHT) {
          leds[getIndex(x + 2 + xOffsets[charIdx], 4 + y)] = CRGB::White;
        }
      }
    }
  }
}