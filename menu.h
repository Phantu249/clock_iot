#ifndef MENU_H
#define MENU_H

#include "screen.h"

// typedef enum : uint8_t {
// OFF = 0,       // 0: Tắt (Black)
// RED = 1,       // 1: Đỏ
// GREEN = 2,     // 2: Xanh lá
// BLUE = 3,      // 3: Xanh dương
// YELLOW = 4,    // 4: Vàng
// CYAN = 5,      // 5: Xanh lam
// MAGENTA = 6,   // 6: Tím hồng
// WHITE = 7,     // 7: Trắng
// ORANGE = 8,    // 8: Cam
// PURPLE = 9,    // 9: Tím
// PINK = 10,     // 10: Hồng
// LIME = 11,     // 11: Xanh chanh
// TEAL = 12,     // 12: Xanh ngọc
// VIOLET = 13,   // 13: Tím nhạt
// GOLD = 14,     // 14: Vàng kim
// SILVER = 15    // 15: Bạc
// } COLOR;

typedef enum : uint8_t {
  TIME_SETTING,
  BLUETOOTH_SETTING,
  GAME1,
  MENU_OPTION_COUNT 
} MENU_OPTION;

typedef enum : uint8_t {
UP,
DOWN
}SLIDE_DIRECTION ;

class Menu : public Screen {
  protected:
    MENU_OPTION _currentOption;
    MENU_OPTION _nextOption;
    bool _isSliding;
    int _slideOffset;
    SLIDE_DIRECTION _slideDirection;
    CRGB _slideFrame[NUM_LEDS];

  public:
    Menu();
    CRGB* draw() override;
    void renderFrame(MENU_OPTION option, CRGB* frame);
    void onButton(Button button, Callback callback = NULL) override;
    void onButtonUp() override;
    void onButtonDown() override;
    void onButtonLeft() override;
    void onButtonRight() override;
    void onButtonRight(Callback callback);
    uint8_t getFrame() override;
    int getIndex(uint8_t x, uint8_t y);
};

#endif