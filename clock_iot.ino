#include <FastLED.h>
#include "config.h"
#include "menu.h"
#include "ble.h"
#include "clock.h"
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "custom_screen.h"

String ssid;
String password;
Preferences preferences;

static const uint32_t WIFI_TIMEOUT_MS = 10000;

uint32_t timestamp = 0;
uint64_t lastUpdate = 0;
int8_t offset = 7;
char *ntpServer = "pool.ntp.org";

CRGB frame[NUM_LEDS];       // 1D array for FastLED show
Screen *screen;             // A pointer for main screen
State appState;             // A current state of the app
QueueHandle_t frameQueue;   // A queue receive a frame (an array to show)
QueueHandle_t buttonQueue;  // A queue revice a button
Ble *bleScreen;             // A BLE screen init default by app
TimeMode timeMode;

// Cấu trúc lưu thông tin debounce cho mỗi nút
struct ButtonInfo {
  int pin;
  volatile unsigned long lastDebounceTime;
};

// Mảng thông tin debounce cho các nút
ButtonInfo buttonInfos[] = {
  { BUTTON_UP, 0 },
  { BUTTON_DOWN, 0 },
  { BUTTON_LEFT, 0 },
  { BUTTON_RIGHT, 0 },
  { BUTTON_BACK, 0 },
  { BUTTON_MENU, 0 }
};
const int numButtons = sizeof(buttonInfos) / sizeof(buttonInfos[0]);
const unsigned long debounceDelay = 200;  // Độ trễ debounce (ms)

// Hàm ISR chung cho các nút
void IRAM_ATTR button_isr_handler(void *arg) {
  ButtonInfo *btnInfo = (ButtonInfo *)arg;
  unsigned long currentTime = millis();

  // Kiểm tra debounce
  if (currentTime - btnInfo->lastDebounceTime > debounceDelay) {
    Button btn;
    if (btnInfo->pin == BUTTON_UP) btn = BUTTON_UP;
    else if (btnInfo->pin == BUTTON_DOWN) btn = BUTTON_DOWN;
    else if (btnInfo->pin == BUTTON_LEFT) btn = BUTTON_LEFT;
    else if (btnInfo->pin == BUTTON_RIGHT) btn = BUTTON_RIGHT;
    else if (btnInfo->pin == BUTTON_BACK) btn = BUTTON_BACK;
    else if (btnInfo->pin == BUTTON_MENU) btn = BUTTON_MENU;

    xQueueSendFromISR(buttonQueue, &btn, NULL);
    btnInfo->lastDebounceTime = currentTime;
  }
}

// Orverdrive onWrite to handle message receive from BLE Charateristic
class CustomBLECharacteristicCallbacks : public NimBLECharacteristicCallbacks {
public:
  void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
    std::string value = pCharacteristic->getValue();

    NimBLEUUID uuid = pCharacteristic->getUUID();

    if (uuid.equals(NimBLEUUID(CUSTOM_SCREEN_CHARACTERISTIC_UUID))) {
      customScreenBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(TIME_CHARACTERISTIC_UUID))) {
      timeBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(ALARM_CHARACTERISTIC_UUID))) {
      Serial.println("Received ALARM characteristic");

    } else if (uuid.equals(NimBLEUUID(BUTTON_CHARACTERISTIC_UUID))) {
      buttonBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(WIFI_CHARACTERISTIC_UUID))) {
      wifiBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(TIMEMODE_CHARACTERISTIC_UUID))) {
      timeModeBLEHandler(value);

    } else {
      Serial.println("Unknown characteristic UUID");
    }

    pCharacteristic->setValue("");
  };
};

void customScreenBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive custom screen data: %s", value);

  if (value.length() != 256) {
    Serial.println("\n[ERROR]: Invalid length for LED data");
    return;
  }

  preferences.begin("custom_screen", false);

  uint8_t totalFrame = preferences.getUChar("total_frame", 0);
  if (totalFrame >= MAX_FRAME) {
    Serial.printf("\n[INFO]: Clear old frames and start new set of frames");

    preferences.clear();
    preferences.putUChar("total_frame", 1);
    totalFrame = 1;
  } else {
    totalFrame += 1;

    Serial.printf("\n[INFO]: New total frames in flash: %d", totalFrame);
    preferences.putUChar("totalframe", totalFrame);
  }

  Serial.printf("\n[INFO]: Add frame %d", totalFrame);

  String name = "frame" + String(totalFrame - 1);
  preferences.putBytes(name.c_str(), value.data(), value.length());

  preferences.end();
}


void timeBLEHandler(std::string value) {
  memcpy(&timestamp, value.data(), sizeof(uint32_t));
  Serial.printf("Timestamp: %d ", timestamp);
}

void timeModeBLEHandler(std::string value) {
  if (value.length() < 1) return;
  Serial.printf("\n Set time mode to %s", value.c_str());
  timeMode = static_cast<TimeMode>(atoi(value.c_str()));

  preferences.begin("app", false);  // false = read/write
  preferences.putUChar("TIMEMODE", timeMode);
  preferences.end();

  switch (timeMode) {
    case TimeMode::AUTO:
      getTime();
      break;
    case TimeMode::MANUAL:
      break;
    default:
      return;
  }
}

void alarmBLEHandler(std::string value) {
}

void wifiBLEHandler(std::string value) {
  // Chuyển std::string sang String
  String jsonString = String(value.c_str());

  // Khởi tạo bộ phân tích JSON
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.print("Lỗi phân tích JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Lấy ssid và password từ JSON
  const char *newSsid = doc["ssid"];
  const char *newPassword = doc["password"];

  Serial.println("SSID: " + String(newSsid));
  Serial.println("Password: " + String(newPassword));

  // Lưu vào Preferences
  preferences.begin("app", false);  // false = read/write
  preferences.putString("SSID", newSsid);
  preferences.putString("PASSWORD", newPassword);
  preferences.end();

  ssid = String(newSsid);
  password = String(newPassword);

  Serial.println("Đã lưu ssid và password vào Preferences!");
}

void buttonBLEHandler(std::string value) {
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

void getTime() {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("Try to connecting WiFi %s \n", ssid.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println("\nWiFi timed out – skipping NTP sync");
      return;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  int32_t gmtOffsetSec = offset * 3600;
  configTime(gmtOffsetSec, 0, ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to get NTP time – leaving timestamp as is");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return;
  }

  timestamp = mktime(&timeinfo);
  lastUpdate = millis();
  Serial.print("NTP sync timestamp: ");
  Serial.println(timestamp);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(9600);

  preferences.begin("app", true);  //read only
  ssid = preferences.getString("SSID", "YOUR-SSID");
  password = preferences.getString("PASSWORD", "YOUR-PASSWORD");
  timeMode = static_cast<TimeMode>(preferences.getUChar("TIMEMODE", TimeMode::MANUAL));

  Serial.printf("System run at %s mode \n", timeMode == TimeMode::AUTO ? "AUTO" : "MANUAL");

  if (timeMode == TimeMode::AUTO) getTime();

  // Init default value
  appState = State::CLOCK;
  screen = new Clock(timestamp, offset);
  bleScreen = new Ble(new CustomBLECharacteristicCallbacks());

  // Init queue
  frameQueue = xQueueCreate(4, sizeof(CRGB *));
  buttonQueue = xQueueCreate(10, sizeof(uint8_t));

  FastLED.addLeds<LED_TYPE, LED_PIN>(frame, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Cấu hình các nút (attach Interrupt)
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonInfos[i].pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(buttonInfos[i].pin), button_isr_handler, &buttonInfos[i], FALLING);
  }

  //Task create
  xTaskCreate(uiTask, "UI Task", 4096, NULL, 1, NULL);
  xTaskCreate(drawTask, "Draw Task", 4096, NULL, 1, NULL);
  xTaskCreate(controllerTask, "Controller Task", 2048, NULL, 1, NULL);
  xTaskCreate(intervalTimeUpdater, "Interval Time Updater", 2048, NULL, 1, NULL);
}

// Controller task receive button form queue to make a decision based on state
void controllerTask(void *param) {
  Button btn;
  while (1) {
    if (xQueueReceive(buttonQueue, &btn, portMAX_DELAY)) {
      switch (appState) {
        case CLOCK:
          switch (btn) {
            case BUTTON_BACK:
              break;
            case BUTTON_MENU:
              delete screen;
              screen = new Menu();
              appState = MENU;
              break;
            case BUTTON_LEFT:
            case BUTTON_RIGHT:
              break;
            case BUTTON_DOWN:
            case BUTTON_UP:
              delete screen;
              screen = new CustomScreen();
              appState = CUSTOM;
              break;
          }
          break;
        case MENU:
          switch (btn) {
            case BUTTON_BACK:
              delete screen;
              screen = new Clock(timestamp, offset);
              appState = CLOCK;
              break;
            case BUTTON_MENU:
            case BUTTON_LEFT:
            case BUTTON_RIGHT:
            case BUTTON_DOWN:
            case BUTTON_UP:
              screen->onButton(btn, callback);
              break;
          }
          break;
        case CUSTOM:
          switch (btn) {
            case BUTTON_BACK:
              delete screen;
              screen = new Clock(timestamp, offset);
              appState = CLOCK;
              break;
            case BUTTON_MENU:
            case BUTTON_LEFT:
            case BUTTON_RIGHT:
            case BUTTON_DOWN:
            case BUTTON_UP:
              screen->onButton(btn);
              break;
          }
          break;
        case GAME:
          break;
        case BLE:
          switch (btn) {
            case BUTTON_BACK:
              screen = new Menu();
              appState = MENU;
              bleScreen->stopAdvertising();
              break;
            case BUTTON_MENU:
            case BUTTON_LEFT:
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

// Callback func is used in case Menu screen want to change state by choosing an option
void callback(void *s, State newState) {
  // Dont delete BleScreen because its always kept by app
  if (appState != BLE) {
    delete screen;
  }

  screen = s ? static_cast<Screen *>(s) : bleScreen;
  appState = newState;

  // change state to Ble -> start advertising ble
  if (newState == BLE) {
    bleScreen->startAdvertising();
  }
}


// Task draw current frame, send it to queue
void uiTask(void *param) {
  while (1) {
    if (screen != NULL) {
      CRGB *src = screen->draw();
      if (src != NULL && xQueueSend(frameQueue, &src, 0) != pdTRUE) {
        Serial.println("Warning: Frame queue full, frame dropped");
      }
      vTaskDelay(pdMS_TO_TICKS(1000 / screen->getFrame()));
    } else {
      vTaskDelay(1000);
    }
  }
}


// Task receive frame from queue and show it
void drawTask(void *param) {
  CRGB *received;
  while (1) {
    if (xQueueReceive(frameQueue, &received, portMAX_DELAY)) {
      memcpy(frame, received, sizeof(CRGB) * NUM_LEDS);
      FastLED.show();
    }
  }
}

void intervalTimeUpdater(void *param) {
  for (;;) {
    uint32_t currentMillis = millis();

    uint32_t elapsedMillis = currentMillis - lastUpdate;
    uint32_t secondsPassed = elapsedMillis / 1000;
    timestamp += secondsPassed;
    lastUpdate += secondsPassed * 1000;

    if (appState == State::CLOCK) {
      static_cast<Clock *>(screen)->setTimestamp(timestamp);
    }

    if (timeMode == TimeMode::AUTO && timestamp % 86400 == 0 && timestamp > 0) {
      getTime();
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}