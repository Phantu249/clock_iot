#include <FastLED.h>
#include "config.h"
#include "menu.h"
#include "ble.h"
#include <NimBLEDevice.h>

CRGB frame[NUM_LEDS];
Screen *screen;
State appState;
QueueHandle_t frameQueue;
QueueHandle_t buttonQueue;
Ble *bleScreen;
uint8_t dataArray[256];


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

class CustomBLECharacteristicCallbacks : public NimBLECharacteristicCallbacks {
public:
  void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
    std::string value = pCharacteristic->getValue();

    if (pCharacteristic->getUUID() == NimBLEUUID("1001")) {
      // Dữ liệu LED (byte array)
      if (value.length() != 256) {
        Serial.println("Invalid length for LED data");
        return;
      }
      memcpy(dataArray, value.data(), 256);
      renderCustom();
    }
    else if (pCharacteristic->getUUID() == NimBLEUUID("1002")) {
      // Dữ liệu thời gian
      // if (value.length() != 4) {
      //   Serial.println("Invalid time format");
      //   return;
      // }
      uint32_t timestamp;
      memcpy(&timestamp, value.data(), sizeof(uint32_t));
      Serial.printf("Timestamp: %d ", timestamp);
    }
    else if (pCharacteristic->getUUID() == NimBLEUUID("1003")) {
      // Nút nhấn (button)
      if (value.length() < 1) {
        Serial.println("Invalid button data");
        return;
      }
      uint8_t buttonId = value[0];
      Button btn = static_cast<Button>(buttonId);
      if (xQueueSend(buttonQueue, &btn, 0) == pdTRUE) {
          Serial.printf("Button %d enqueued.\n", btn);
        } else {
          Serial.println("Queue full, button dropped.");
        }
    }
    else {
      Serial.println("Unknown characteristic UUID");
    }

    // Clear sau khi xử lý xong
    pCharacteristic->setValue("");
  }
};


int getIndex(uint8_t x, uint8_t y) {
  uint8_t physicalY = NUM_ROWS - 1 - y;  // y=0 -> physicalY=15, y=15 -> physicalY=0
  if (physicalY % 2 == 0) {
    return physicalY * NUM_COLS + x;  // Hàng chẵn: x tăng (trái -> phải)
  } else {
    return physicalY * NUM_COLS + (NUM_COLS - 1 - x);  // Hàng lẻ: x giảm (phải -> trái)
  }
}


void renderCustom() {
      for (int y = 0; y < NUM_ROWS; y++) {
        for (int x = 0; x < NUM_COLS; x++) {
            int index = getIndex(x, y);
            int pixelIndex = y * NUM_COLS + x; // 1 byte mỗi pixel
            uint8_t colorIndex = dataArray[pixelIndex]; // Lấy giá trị 0-15
            frame[index] = colors[colorIndex];
        }
    }
    FastLED.show();
}

void setup() {
  Serial.begin(9600);

  appState = MENU;
  screen = new Menu();
  bleScreen = new Ble(new CustomBLECharacteristicCallbacks());

  frameQueue = xQueueCreate(4, sizeof(CRGB *));  // queue chứa con trỏ
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
  Button btn;
  while (1) {
    if (xQueueReceive(buttonQueue, &btn, portMAX_DELAY)) {
      // Xử lý sự kiện nút nhấn
      switch (appState) {
        case CLOCK:
          switch (btn) {
            case BUTTON_LEFT:
              break;
            case BUTTON_RIGHT:
              delete screen;
              screen = new Menu();
              appState = MENU;
              break;
            case BUTTON_DOWN:
            case BUTTON_UP:
              screen->onButton(btn);
              break;
          }
          break;
        case MENU:
          switch (btn) {
            case BUTTON_LEFT:
              // delete screen;
              // screen = new Clock();
              // appState = CLOCK;
              break;
            case BUTTON_RIGHT:
            case BUTTON_DOWN:
            case BUTTON_UP:
              screen->onButton(btn, callback);
              break;
          }
          break;
        case GAME:
          break;
        case BLE:
          switch (btn) {
            case BUTTON_LEFT:
              // delete screen;
              screen = new Menu();
              appState = MENU;
              bleScreen->stopAdvertising();
              break;
            case BUTTON_RIGHT:
            case BUTTON_DOWN:
            case BUTTON_UP:
              screen->onButton(btn);
              break;
          }
          break;
      }
    }
  }
}

void callback(void *s, State newState) {
  if (appState != BLE) {
    delete screen;
  }

  screen = s ? static_cast<Screen *>(s) : bleScreen;
  appState = newState;

  if (newState == BLE) {
    bleScreen->startAdvertising();
  }
}


void uiTask(void *param) {
  while (1) {
    if (screen != NULL) {
      CRGB *src = screen->draw();
      // Kiểm tra lỗi khi gửi queue
      if (xQueueSend(frameQueue, &src, 0) != pdTRUE) {
        Serial.println("Warning: Frame queue full, frame dropped");
      }
      vTaskDelay(pdMS_TO_TICKS(1000 / screen->getFrame()));
    } else {
      vTaskDelay(1000);
    }
  }
}

void drawTask(void *param) {
  CRGB *received;

  while (1) {
    if (xQueueReceive(frameQueue, &received, portMAX_DELAY)) {
      memcpy(frame, received, sizeof(CRGB) * NUM_LEDS);
      FastLED.show();
    }
  }
}

void loop() {
}
