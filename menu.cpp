#include "menu.h"

Menu::Menu() {
    _frame = 5; // Ví dụ: 30 FPS

    // Khởi tạo các frame cho từng tùy chọn menu
    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            _frames[i][j] = CRGB::Black; // Mặc định tất cả LED tắt
        }
    }

    // Tùy chỉnh frame cho TIME_SETTING
    // Ví dụ: Hiển thị chữ "TIME" hoặc biểu tượng đồng hồ
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            // Thiết kế mẫu cho TIME_SETTING (có thể dùng bitmap)
            if ((i == 4 && j >= 4 && j <= 11) || (i == 8 && j >= 4 && j <= 11)) {
                _frames[TIME_SETTING][i * 16 + j] = CRGB::Blue; // Ví dụ: Màu xanh
            }
        }
    }

    // Tùy chỉnh frame cho BLUETOOTH_SETTING
    // Ví dụ: Hiển thị biểu tượng Bluetooth
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            // Thiết kế mẫu cho Bluetooth (ví dụ: hình chữ B đơn giản)
            if ((i == 6 && j == 8) || (i == 7 && j == 7) || (i == 7 && j == 9) ||
                (i == 8 && j == 8) || (i == 9 && j == 7) || (i == 9 && j == 9)) {
                _frames[BLUETOOTH_SETTING][i * 16 + j] = CRGB::Cyan; // Ví dụ: Màu cyan
            }
        }
    }

    // Tùy chỉnh frame cho GAME1
    // Ví dụ: Hiển thị biểu tượng game
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            // Thiết kế mẫu cho GAME1 (ví dụ: hình vuông)
            if (i >= 6 && i <= 9 && j >= 6 && j <= 9) {
                _frames[GAME1][i * 16 + j] = CRGB::Red; // Ví dụ: Màu đỏ
            }
        }
    }
}


int Menu::getIndex(uint8_t x, uint8_t y) {
  if (y % 2 == 0) {
    return y * NUM_COLS + x;
  } else {
    return y * NUM_COLS + (NUM_COLS - 1 - x);
  }
}

CRGB* Menu::draw() {
  if (_isSliding) {
    for (uint8_t y = 0; y < NUM_ROWS; y++) {
      for (uint8_t x = 0; x < NUM_COLS; x++) {
        int virtualY1 = y + _slideOffset;
        int virtualY2 = y + _slideOffset - NUM_ROWS;

        int index = getIndex(x, y);
        _slideFrame[index] = CRGB::Black;

        // Lấy từ frame hiện tại (trượt lên)
        if (virtualY1 >= 0 && virtualY1 < NUM_ROWS) {
          int srcIndex = getIndex(x, virtualY1);
          _slideFrame[index] = _frames[_currentOption][srcIndex];
        }

        // Lấy từ frame tiếp theo (xuất hiện từ dưới)
        if (virtualY2 >= 0 && virtualY2 < NUM_ROWS) {
          int srcIndex = getIndex(x, virtualY2);
          _slideFrame[index] = _frames[_nextOption][srcIndex];
        }
      }
    }

    _slideOffset += 1;

    if (_slideOffset >= NUM_ROWS) {
      _currentOption = _nextOption;
      _isSliding = false;
      memcpy(_slideFrame, _frames[_currentOption], sizeof(_slideFrame));
    }

    return _slideFrame;
  }

  return _frames[_currentOption];
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
  _nextOption = static_cast<MENU_OPTION>((_currentOption + 1) % MENU_OPTION_COUNT);
  _slideOffset = 0;
  _isSliding = true;
}

void Menu::onButtonDown() {
  _nextOption = static_cast<MENU_OPTION>((_currentOption + MENU_OPTION_COUNT - 1) % MENU_OPTION_COUNT);
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
        // s = new TimeSettingScreen(); // bạn thêm ở đây nếu cần
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
