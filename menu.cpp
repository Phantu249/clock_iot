#include "menu.h"

// Frame 16x16 cho TIME_SETTING (256 bytes, 1 byte/pixel)
const uint8_t timeSettingFrame[] PROGMEM = {
    0,0,0,0,0,15,0,0,0,0,15,0,0,0,0,0,
    0,0,0,0,15,0,15,0,0,15,0,15,0,0,0,0,
    0,0,0,15,0,0,0,0,0,0,0,0,15,0,0,0,
    0,0,0,0,0,15,0,0,0,0,15,0,0,0,0,0,
    0,15,0,0,0,0,15,15,15,0,0,0,0,15,0,0,
    15,0,0,0,0,0,0,15,0,0,0,0,0,0,15,0,
    0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,
    0,0,15,0,0,0,15,15,15,0,0,0,15,0,0,0,
    0,0,15,0,0,0,15,15,15,0,0,0,15,0,0,0,
    0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,
    15,0,0,0,0,0,0,15,0,0,0,0,0,0,15,0,
    0,15,0,0,0,0,15,15,15,0,0,0,0,15,0,0,
    0,0,0,0,0,15,0,0,0,0,15,0,0,0,0,0,
    0,0,0,15,0,0,0,0,0,0,0,0,15,0,0,0,
    0,0,0,0,15,0,15,0,0,15,0,15,0,0,0,0,
    0,0,0,0,0,15,0,0,0,0,15,0,0,0,0,0
};

// Frame 16x16 cho BLUETOOTH_SETTING (256 bytes, 1 byte/pixel)
const uint8_t bluetoothSettingFrame[] PROGMEM = {
    0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,
    0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,
    0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,
    0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,
    0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,0,
    0,3,0,0,0,0,0,3,0,0,0,0,0,3,0,0,
    3,0,0,0,0,3,3,3,3,0,0,0,0,0,3,0,
    3,0,0,0,0,3,3,3,3,0,0,0,0,0,3,0,
    0,3,0,0,0,0,0,3,0,0,0,0,0,3,0,0,
    0,0,3,0,0,0,0,0,0,0,0,0,3,0,0,0,
    0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,
    0,0,0,0,3,0,0,0,0,0,3,0,0,0,0,0,
    0,0,0,0,0,3,0,0,0,3,0,0,0,0,0,0,
    0,0,0,0,0,0,3,0,3,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0
};

// Frame 16x16 cho GAME1 (256 bytes, 1 byte/pixel)
const uint8_t game1Frame[] PROGMEM = {
    0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
    0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
    0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,
    0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

CRGB colors[16] = {
        CRGB::Black,   // OFF (0)
        CRGB::Red,     // RED (1)
        CRGB::Green,   // GREEN (2)
        CRGB::Blue,    // BLUE (3)
        CRGB::Yellow,  // YELLOW (4)
        CRGB::Cyan,    // CYAN (5)
        CRGB::Magenta, // MAGENTA (6)
        CRGB::White,   // WHITE (7)
        CRGB::Orange,  // ORANGE (8)
        CRGB::Purple,  // PURPLE (9)
        CRGB::Pink,    // PINK (10)
        CRGB::Lime,    // LIME (11)
        CRGB::Teal,    // TEAL (12)
        CRGB::Violet,  // VIOLET (13)
        CRGB::Gold,    // GOLD (14)
        CRGB::Silver   // SILVER (15)
    };

Menu::Menu() {
  _frame = 20;
  _currentOption = TIME_SETTING;
  _isSliding = false;
  _slideOffset = 0;
  _slideDirection = UP;
  for (int j = 0; j < NUM_LEDS; j++) {
    _slideFrame[j] = CRGB::Black;
  }
}

int Menu::getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;  // y=0 -> physicalY=15, y=15 -> physicalY=0
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;  // Hàng chẵn: x tăng (trái -> phải)
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);  // Hàng lẻ: x giảm (phải -> trái)
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
            int pixelIndex = y * NUM_COLS + x; // 1 byte mỗi pixel
            uint8_t colorIndex = pgm_read_byte(&frameData[pixelIndex]); // Lấy giá trị 0-15
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

    for (uint8_t y = 0; y < NUM_ROWS; y++) {
      for (uint8_t x = 0; x < NUM_COLS; x++) {
        int virtualY1, virtualY2;
        if (_slideDirection == UP) {
          // Trượt lên: frame hiện tại đi lên, frame tiếp theo vào từ dưới
          virtualY1 = y - _slideOffset;             // Frame hiện tại trượt lên
          virtualY2 = y - _slideOffset + NUM_ROWS;  // Frame tiếp theo vào từ dưới
        } else {
          // Trượt xuống: frame hiện tại đi xuống, frame tiếp theo vào từ trên
          virtualY1 = y + _slideOffset;             // Frame hiện tại trượt xuống
          virtualY2 = y + _slideOffset - NUM_ROWS;  // Frame tiếp theo vào từ trên
        }

        int index = getIndex(x, y);
        _slideFrame[index] = CRGB::Black;

        if (virtualY1 >= 0 && virtualY1 < NUM_ROWS) {
          int srcIndex = getIndex(x, virtualY1);
          _slideFrame[index] = currentFrame[srcIndex];
        }

        if (virtualY2 >= 0 && virtualY2 < NUM_ROWS) {
          int srcIndex = getIndex(x, virtualY2);
          _slideFrame[index] = nextFrame[srcIndex];
        }
      }
    }

    _slideOffset += 1;

    if (_slideOffset >= NUM_ROWS) {
      _currentOption = _nextOption;
      _isSliding = false;
      renderFrame(_currentOption, _slideFrame);
    }

    return _slideFrame;
  }

  renderFrame(_currentOption, _slideFrame);
  return _slideFrame;
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
      break;
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
  if (callback) {
    Screen* s = NULL;
    switch (_currentOption) {
      case TIME_SETTING:
        // s = new TimeSettingScreen(); // Thêm nếu cần
        break;
      case BLUETOOTH_SETTING:
        break;
      case GAME1:
        break;
      default:
        break;
    }
    if (s) callback(s);
  }
}

uint8_t Menu::getFrame() {
  return _frame;
}