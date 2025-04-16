#include <FastLED.h>
#include "config.h"
#include "menu.h"

CRGB frame[NUM_LEDS];
Screen* screen;
State appState;
QueueHandle_t frameQueue;
QueueHandle_t buttonQueue;

void IRAM_ATTR button_isr_handler_up() {
    Button btn = BUTTON_UP;
    xQueueSendFromISR(buttonQueue, &btn, NULL);
}

void IRAM_ATTR button_isr_handler_down() {
    Button btn = BUTTON_DOWN;
    xQueueSendFromISR(buttonQueue, &btn, NULL);
}

void IRAM_ATTR button_isr_handler_left() {
    Button btn = BUTTON_LEFT;
    xQueueSendFromISR(buttonQueue, &btn, NULL);
}

void IRAM_ATTR button_isr_handler_right() {
    Button btn = BUTTON_RIGHT;
    xQueueSendFromISR(buttonQueue, &btn, NULL);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial start at 9600");

  appState = MENU;
  screen = new Menu();
  
  frameQueue = xQueueCreate(4, sizeof(CRGB*));  // queue chứa con trỏ
  buttonQueue = xQueueCreate(10, sizeof(uint8_t));

  FastLED.addLeds<LED_TYPE, LED_PIN>(frame, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  attachInterrupt(BUTTON_UP, button_isr_handler_up, FALLING);
  attachInterrupt(BUTTON_DOWN, button_isr_handler_down, FALLING);
  attachInterrupt(BUTTON_LEFT, button_isr_handler_left, FALLING);
  attachInterrupt(BUTTON_RIGHT, button_isr_handler_right, FALLING);

  xTaskCreate(uiTask, "UI Task", 4096, NULL, 1, NULL);
  xTaskCreate(drawTask, "Draw Task", 4096, NULL, 1, NULL);
  xTaskCreate(controllerTask, "Controller Task", 2048, NULL, 1, NULL);
}

void controllerTask(void *param) {
    Button btn_id;
    while (1) {
        if (xQueueReceive(buttonQueue, &btn_id, portMAX_DELAY)) {
            // Xử lý sự kiện nút nhấn
            switch (btn_id) {
              case BUTTON_UP:
                break;
              case BUTTON_DOWN:
                break;
              case BUTTON_LEFT:
                break;
              case BUTTON_RIGHT:
                break;
            }
        }
    }
}

// void controllerTask(void *param) {
//     Button btn;
//     while (1) {
//         if (xQueueReceive(buttonQueue, &btn, portMAX_DELAY)) {
//             // Xử lý sự kiện nút nhấn
//             switch (appState) {
//               case CLOCK:
//                 switch (btn) {
//                   case BUTTON_LEFT:
//                     break;
//                   case BUTTON_RIGHT:
//                     delete screen;
//                     screen = new Menu();
//                     appState = MENU;
//                     break;
//                   case BUTTON_DOWN:
//                   case BUTTON_LEFT:
//                     screen->onButton(btn);
//                     break;
//                 }
//                 break;
//               case MENU:
//                 switch (btn) {
//                   case BUTTON_LEFT:
//                     delete screen;
//                     screen = new Clock();
//                     appState = CLOCK;
//                     break;                  
//                   case BUTTON_RIGHT:
//                   case BUTTON_DOWN:
//                   case BUTTON_LEFT:
//                     screen->onButton(btn);
                    
//                     break;
//                 }
//                 break;
//               case GAME:
//                 break;
//             }
//         }
//     }
// }

void callback(void *s) {
  delete screen;
  screen = static_cast<Screen*>(s);
}


void uiTask(void *param) {
    while (1) {
      if (screen != NULL) {
        CRGB* src = screen->draw();
        // Gửi vào queue (nếu đầy thì giải phóng)
        xQueueSend(frameQueue, &src, 0);
        vTaskDelay(pdMS_TO_TICKS(1000/screen->getFrame())); // ~30 FPS
      }
  }
}

void drawTask(void *param) {
  CRGB* received;

  while(1) {
    if (xQueueReceive(frameQueue, &received, portMAX_DELAY)) {
      memcpy(frame, received, sizeof(CRGB) * NUM_LEDS);
      FastLED.show();
    }
  }
}

void loop() {
}
