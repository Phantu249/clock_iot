#include "menu.h"
#include "ble.h"
#include "config.h"

// Frame 16x16 cho TIME_SETTING (256 bytes, 1 byte/pixel)
const uint8_t timeSettingFrame[] PROGMEM = {
  0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 15, 0, 15, 0, 0, 15, 0, 15, 0, 0, 0, 0,
  0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0,
  0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0,
  0, 15, 0, 0, 0, 0, 15, 15, 15, 0, 0, 0, 0, 15, 0, 0,
  15, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 15, 0,
  0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 15, 0, 0, 0, 15, 15, 15, 0, 0, 0, 15, 0, 0, 0,
  0, 0, 15, 0, 0, 0, 15, 15, 15, 0, 0, 0, 15, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0,
  15, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 15, 0,
  0, 15, 0, 0, 0, 0, 15, 15, 15, 0, 0, 0, 0, 15, 0, 0,
  0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0,
  0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0,
  0, 0, 0, 0, 15, 0, 15, 0, 0, 15, 0, 15, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0
};

// Frame 16x16 cho BLUETOOTH_SETTING (256 bytes, 1 byte/pixel)
const uint8_t bluetoothSettingFrame[] PROGMEM = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 3, 3, 7, 3, 3, 3, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 3, 7, 7, 3, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 3, 7, 3, 7, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 7, 3, 7, 3, 3, 7, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 7, 7, 3, 7, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 3, 7, 7, 3, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 7, 7, 3, 7, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 7, 3, 7, 3, 3, 7, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 3, 7, 3, 7, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 3, 3, 3, 7, 7, 3, 3, 3, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 3, 3, 7, 3, 3, 3, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Frame 16x16 cho GAME1 (256 bytes, 1 byte/pixel)
const uint8_t game1Frame[] PROGMEM = {
  0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Menu::Menu() {
  _frame = 10;
  _currentOption = TIME_SETTING;
  _isSliding = false;
  _slideOffset = 0;
  _slideDirection = UP;
}

// getIndex func for convert x,y index from 2D array to 1D CRGB array use for FastLED
int Menu::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);
  }
}


void Menu::renderFrame(MENU_OPTION option, CRGB* frame) {
  const uint8_t* frameData;

  switch (option) {
    case TIME_SETTING:
      frameData = timeSettingFrame;
      break;
    case BLUETOOTH_SETTING:
      frameData = bluetoothSettingFrame;
      break;
    case GAME1:
      frameData = game1Frame;
      break;
    default:
      return;
  }

  for (int j = 0; j < NUM_LEDS; j++) {
    frame[j] = CRGB::Black;
  }

  for (int y = 0; y < NUM_ROWS; y++) {
    for (int x = 0; x < NUM_COLS; x++) {
      int index = getIndex(x, y);
      int pixelIndex = y * NUM_COLS + x;                           // 1 byte mỗi pixel
      uint8_t colorIndex = pgm_read_byte(&frameData[pixelIndex]);  // Lấy giá trị 0-15
      frame[index] = colors[colorIndex];
    }
  }
}

CRGB* Menu::draw() {
  if (_isSliding) {
    CRGB currentFrame[NUM_LEDS];
    CRGB nextFrame[NUM_LEDS];

    renderFrame(_currentOption, currentFrame);
    renderFrame(_nextOption, nextFrame);

    // Tính delta time
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdate) / 1000.0f;  // Chuyển sang giây
    lastUpdate = currentTime;

    // Cập nhật slideOffset dựa trên tốc độ và delta time
    _slideOffset += slideSpeed * deltaTime;

    for (uint8_t y = 0; y < NUM_ROWS; y++) {
      for (uint8_t x = 0; x < NUM_COLS; x++) {
        int virtualY1, virtualY2;
        if (_slideDirection == UP) {
          virtualY1 = y - (int)_slideOffset;             // Frame hiện tại trượt lên
          virtualY2 = y - (int)_slideOffset + NUM_ROWS;  // Frame tiếp theo vào từ dưới
        } else {
          virtualY1 = y + (int)_slideOffset;             // Frame hiện tại trượt xuống
          virtualY2 = y + (int)_slideOffset - NUM_ROWS;  // Frame tiếp theo vào từ trên
        }

        int index = getIndex(x, y);
        frame[index] = CRGB::Black;

        // Gán đúng frame cho virtualY1 và virtualY2
        if (_slideDirection == UP) {
          if (virtualY1 >= 0 && virtualY1 < NUM_ROWS) {
            int srcIndex = getIndex(x, virtualY1);
            frame[index] = currentFrame[srcIndex];
          }
          if (virtualY2 >= 0 && virtualY2 < NUM_ROWS) {
            int srcIndex = getIndex(x, virtualY2);
            frame[index] = nextFrame[srcIndex];
          }
        } else {
          if (virtualY1 >= 0 && virtualY1 < NUM_ROWS) {
            int srcIndex = getIndex(x, virtualY1);
            frame[index] = currentFrame[srcIndex];
          }
          if (virtualY2 >= 0 && virtualY2 < NUM_ROWS) {
            int srcIndex = getIndex(x, virtualY2);
            frame[index] = nextFrame[srcIndex];
          }
        }
      }
    }

    if (_slideOffset >= NUM_ROWS) {
      _currentOption = _nextOption;
      _isSliding = false;
      _slideOffset = 0;  // Reset offset
      renderFrame(_currentOption, frame);
    }

    return frame;
  } else {
    lastUpdate = millis();  // Cập nhật thời gian khi không trượt
    renderFrame(_currentOption, frame);
    return frame;
  }
}

void Menu::onButton(Button button, Callback callback) {
  if (_isSliding) return;
  switch (button) {
    case BUTTON_UP:
      onButtonUp();
      break;
    case BUTTON_DOWN:
      onButtonDown();
      break;
    case BUTTON_LEFT:
      onButtonLeft();
      break;
    case BUTTON_RIGHT:
      if (callback) onButtonRight(callback);
      else onButtonRight();
      break;
    default:
      return;
  }
}

void Menu::onButtonUp() {
  if (_isSliding) return;
  _nextOption = static_cast<MENU_OPTION>((_currentOption + 1) % MENU_OPTION_COUNT);
  _slideDirection = UP;
  _slideOffset = 0;
  _isSliding = true;
}

void Menu::onButtonDown() {
  if (_isSliding) return;
  _nextOption = static_cast<MENU_OPTION>((_currentOption + MENU_OPTION_COUNT - 1) % MENU_OPTION_COUNT);
  _slideDirection = DOWN;
  _slideOffset = 0;
  _isSliding = true;
}

void Menu::onButtonLeft() {}

void Menu::onButtonRight() {}

void Menu::onButtonRight(Callback callback) {
  State newState;
  if (callback) {
    Screen* s = NULL;
    switch (_currentOption) {
      case TIME_SETTING:
        // s = new TimeSettingScreen(); // Thêm nếu cần
        break;
      case BLUETOOTH_SETTING:
        // do nothing
        newState = BLE;
        break;
      case GAME1:
        break;
      default:
        return;
    }
    if (s && _currentOption != BLUETOOTH_SETTING) callback(s, newState);
    else callback(NULL, newState);
  }
}