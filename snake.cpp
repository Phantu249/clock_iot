#include "snake.h"

SnakeGame::SnakeGame() {
  length = 3;
  snake[0] = {5, 5};
  snake[1] = {4, 5};
  snake[2] = {3, 5};
  dx = 1;
  dy = 0;
  _frame = 30; // vẽ 30 lần/giây
  moveInterval = 200; // rắn di chuyển mỗi 200ms (5 lần/giây)
  lastMoveTime = millis();
  gameOver = false;
  generateFood();
}

// 👉 Mapping kiểu zigzag
uint16_t SnakeGame::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = HEIGHT - 1 - y;  // y=0 -> physicalY=15, y=15 -> physicalY=0
  if (physicalY % 2 == 0) {
    return physicalY * WIDTH + x;  // Hàng chẵn: x tăng (trái -> phải)
  } else {
    return physicalY * WIDTH + (WIDTH - 1 - x);  // Hàng lẻ: x giảm (phải -> trái)
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

  for (int i = 0; i < length; ++i) {
    if (newHead == snake[i]) {
      gameOver = true;
      gameOverTime = millis();
      return;
    }
  }
}

CRGB* SnakeGame::draw() {
  unsigned long now = millis();

  if (gameOver) {
    if (!gameOverEffectDone) {
      // Nháy đèn đỏ 3 lần mỗi 200ms
      static int flashCount = 0;
      static bool on = false;
      static unsigned long lastFlash = 0;

      if (now - lastFlash > 200) {
        lastFlash = now;
        on = !on;
        flashCount++;

        fill_solid(leds, WIDTH * HEIGHT, on ? CRGB::Red : CRGB::Black);
      }

      if (flashCount >= 6) {  // 3 lần nháy (on/off)
        gameOverEffectDone = true;
        gameOverTime = now;  // đánh dấu thời gian bắt đầu hiển thị chữ
      }

      return leds;
    }

    // Sau khi nháy xong → hiển thị "GAME OVER"
    if (now - gameOverTime < 2000) {
      fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);
      drawGameOverText();
      return leds;
    }

    return leds; // quay về Clock sẽ được xử lý ở controller
  }

  // Bình thường: rắn di chuyển
  if (now - lastMoveTime >= moveInterval) {
    lastMoveTime = now;
    moveSnake();
  }

  // Vẽ rắn
  fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);
  for (int i = 0; i < length; ++i) {
    if (i == 0)
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Red;
    else
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Green;
  }

  // Vẽ đồ ăn
  leds[getIndex(food.x, food.y)] = CRGB::Yellow;

  return leds;
}

void SnakeGame::onButton(Button button, Callback _c) {
  switch (button) {
    case BUTTON_UP:    onButtonUp(); break;
    case BUTTON_DOWN:  onButtonDown(); break;
    case BUTTON_LEFT:  onButtonLeft(); break;
    case BUTTON_RIGHT: onButtonRight(); break;
  }
}

void SnakeGame::onButtonUp() {
  if (dy == 0) { dx = 0; dy = -1; }
}

void SnakeGame::onButtonDown() {
  if (dy == 0) { dx = 0; dy = 1; }
}

void SnakeGame::onButtonLeft() {
  if (dx == 0) { dx = -1; dy = 0; }
}

void SnakeGame::onButtonRight() {
  if (dx == 0) { dx = 1; dy = 0; }
}

uint8_t SnakeGame::getFrame() {
  return _frame;
}

bool SnakeGame::isGameOver() {
  return gameOver;
}

void SnakeGame::endGame() {
  gameOver = true;
}

void SnakeGame::drawGameOverText() {
  // Ví dụ hiển thị chữ X màu trắng
  for (int i = 0; i < HEIGHT; ++i) {
    leds[getIndex(i, i)] = CRGB::White;
    leds[getIndex(WIDTH - 1 - i, i)] = CRGB::White;
  }
}