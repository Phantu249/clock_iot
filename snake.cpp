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
  generateFood();
}

uint16_t SnakeGame::getIndex(uint8_t x, uint8_t y) {
  return y * WIDTH + x;
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
}

CRGB* SnakeGame::draw() {
  fill_solid(leds, WIDTH * HEIGHT, CRGB::Black);

  // chỉ di chuyển rắn nếu đủ thời gian
  if (millis() - lastMoveTime >= moveInterval) {
    lastMoveTime = millis();
    moveSnake();
  }

  // vẽ rắn
  for (int i = 0; i < length; ++i) {
    if (i == 0)
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Red; // đầu rắn
    else
      leds[getIndex(snake[i].x, snake[i].y)] = CRGB::Green;
  }

  // vẽ đồ ăn
  leds[getIndex(food.x, food.y)] = CRGB::Yellow;

  return leds;
}

void SnakeGame::onButton(Button button) {
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
