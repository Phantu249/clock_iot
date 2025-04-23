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
#include "ring.h"
#include <string.h>
#include "snake.h"

String ssid;
String password;
Preferences preferences;

static const uint32_t WIFI_TIMEOUT_MS = 20000;

uint32_t timestamp = 0;
uint64_t lastUpdate = 0;
int8_t offset = 7;
char* ntpServer = "pool.ntp.org";

CRGB frame[NUM_LEDS];       // 1D array for FastLED show
Screen *screen;             // A pointer for main screen
State appState;             // A current state of the app
QueueHandle_t frameQueue;   // A queue receive a frame (an array to show)
QueueHandle_t buttonQueue;  // A queue revice a button
Ble *bleScreen;             // A BLE screen init default by app
TimeMode timeMode;
Alarm alarms[5];  // Only 5 alarms now

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
void IRAM_ATTR buttonISRHandler(void *arg) {
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
      alarmBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(BUTTON_CHARACTERISTIC_UUID))) {
      buttonBLEHandler(value);

    } else if (uuid.equals(NimBLEUUID(WIFI_CHARACTERISTIC_UUID))) {
      wifiBLEHandler(value, pCharacteristic);

    } else if (uuid.equals(NimBLEUUID(TIMEMODE_CHARACTERISTIC_UUID))) {
      timeModeBLEHandler(value);

    } else {
      Serial.println("\n[ERROR]: Unknown characteristic UUID");
    }

    if (!uuid.equals(NimBLEUUID(WIFI_CHARACTERISTIC_UUID))) {
      pCharacteristic->setValue("");
    }
  };
};

void customScreenBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive custom screen message with value length %d bytes", value.length());

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
    preferences.putUChar("total_frame", totalFrame);
  }

  Serial.printf("\n[INFO]: Add frame %d", totalFrame);

  String name = "frame_" + String(totalFrame - 1);
  preferences.putBytes(name.c_str(), value.data(), value.length());

  preferences.end();
}

void timeBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive set timestamp message with value %s", value.c_str());

  timestamp = atoi(value.c_str());

  Serial.printf("\n[INFO]: Timestamp: %d ", timestamp);
}

void timeModeBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive set time mode message with value %s", value.c_str());

  timeMode = static_cast<TimeMode>(atoi(value.c_str()));

  preferences.begin("app", false);
  preferences.putUChar("time_mode", timeMode);
  preferences.end();

  switch (timeMode) {
    case TimeMode::AUTO:
      getTime();
      break;
    case TimeMode::MANUAL: break;
  }
}

void takeAlarmsOut() {
  preferences.begin("app", true);
  String alarmsStr = preferences.getString("alarms", "");
  preferences.end();

  char *payload[5] = { nullptr };
  int payloadLength;
  char *value_cstr = strdup(alarmsStr.c_str());

  char *token = strtok(value_cstr, &DELIMITER);

  for (payloadLength = 0; token != nullptr && payloadLength < 5; payloadLength += 1) {
    payload[payloadLength] = token;
    token = strtok(nullptr, &DELIMITER);
  }

  for (int i = 0; i < payloadLength; i++) {
    // Check if time format is valid (HH:MM or XX:XX)
    if (strlen(payload[i]) == 5 && payload[i][2] == ':') {
      char hour_str[3] = { payload[i][0], payload[i][1], '\0' };
      char minute_str[3] = { payload[i][3], payload[i][4], '\0' };

      uint8_t hour = atoi(hour_str);
      uint8_t minute = atoi(minute_str);

      alarms[i].hour = hour;
      alarms[i].minute = minute;
      Serial.printf("\n[INFO]: Loaded alarm %02d:%02d", alarms[i].hour, alarms[i].minute);
    }
  }

  free(value_cstr);
}

void alarmBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive set alarm message with value %s", value.c_str());

  char *payload[5] = { nullptr };
  int payloadLength;

  for (int i = 0; i < 5; i++) {
    alarms[i] = { -1, -1 };
  }

  char *value_cstr = strdup(value.c_str());
  char *token = strtok(value_cstr, &DELIMITER);

  for (payloadLength = 0; token != nullptr && payloadLength < 5; payloadLength += 1) {
    payload[payloadLength] = token;
    token = strtok(nullptr, &DELIMITER);
  }

  int validAlarmCount = 0;  // Track number of valid alarms
  for (int i = 0; i < payloadLength; i++) {
    // Check if time format is valid (HH:MM or XX:XX)
    if (strlen(payload[i]) == 5 && payload[i][2] == ':') {
      // Verify hours and minutes are numeric
      bool valid = true;
      for (int j = 0; j < 5; j++) {
        if (j == 2) continue;  // Skip colon
        if (!isdigit(payload[i][j])) {
          valid = false;
          break;
        }
      }

      if (valid) {
        char hour_str[3] = { payload[i][0], payload[i][1], '\0' };
        char minute_str[3] = { payload[i][3], payload[i][4], '\0' };

        // Convert to uint8_t
        uint8_t hour = atoi(hour_str);
        uint8_t minute = atoi(minute_str);

        if (hour <= 23 && minute <= 59) {
          alarms[validAlarmCount].hour = hour;
          alarms[validAlarmCount].minute = minute;
          Serial.printf("\n[INFO]: Saved %02d:%02d", alarms[validAlarmCount].hour, alarms[validAlarmCount].minute);
          validAlarmCount++;
        } else {
          Serial.printf("\n[ERROR]: Invalid alarm format %s", payload[i]);
        }
      } else {
        Serial.printf("\n[ERROR]: Invalid alarm format %s", payload[i]);
      }
    } else {
      Serial.printf("\n[ERROR]: Invalid alarm format %s (malformed)", payload[i]);
    }
  }

  // Build the output string for valid alarms (e.g., "12:00,07:00")
  std::string alarmString = "";
  for (int i = 0; i < validAlarmCount; i++) {
    char timeStr[6];  // Buffer for "XX:XX\0"
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", alarms[i].hour, alarms[i].minute);
    alarmString += timeStr;
    if (i < validAlarmCount - 1) {
      alarmString += ",";
    }
  }


  preferences.begin("app", false);
  preferences.putString("alarms", alarmString.c_str());
  Serial.printf("\n[INFO]: Stored alarms in Preferences: %s", alarmString.c_str());
  preferences.end();

  free(value_cstr);  // Free the mutable copy
}

void wifiBLEHandler(std::string value, NimBLECharacteristic *pCharacteristic) {
  Serial.printf("\n[INFO]: Receive set WiFi message with value %s", value.c_str());

  String jsonString = String(value.c_str());

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.printf("\n[ERROR]: Invalid JSON %s", error.c_str());
    return;
  }

  const char *newSsid = doc["ssid"];
  const char *newPassword = doc["password"];

  Serial.printf("\n[INFO]: Extracted SSID: %s, Password: %s", newSsid, newPassword);
  Serial.printf("\n[INFO]: Try to connect to WiFi %s", newSsid);

  WiFi.begin(newSsid, newPassword);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println("\n[ERROR]: Cannot connect to WiFi - notify error");
      // "1" means error, kinda lazy to introduce new macro
      pCharacteristic->setValue("1");
      pCharacteristic->notify();

      return;
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }

  preferences.begin("app", false);
  preferences.putString("ssid", newSsid);
  preferences.putString("password", newPassword);
  preferences.end();

  // Update global variables
  ssid = String(newSsid);
  password = String(newPassword);

  // "0" means success, kinda lazy to introduce new macro too :D
  pCharacteristic->setValue("0");
  pCharacteristic->notify();

  Serial.print("\n[INFO]: Successfully to update new WiFi!");
}

void buttonBLEHandler(std::string value) {
  Serial.printf("\n[INFO]: Receive button pressed message with value %s", value.c_str());

  Button btn = static_cast<Button>(atoi(value.c_str()));

  if (xQueueSend(buttonQueue, &btn, 0) == pdTRUE) {
    Serial.printf("\n[INFO]: Button %d enqueued", btn);
  } else {
    Serial.print("\n[INFO]: Queue full, button dropped");
  }
}

void getTime() {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("\n[INFO]: Try to connecting WiFi %s to sync time", ssid.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      // figure out a way to notify for user that wifi is wrong or cannot connect.
      Serial.println("\n[ERROR]: WiFi timed out – skipping NTP sync");
      return;
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }

  Serial.println("\n[INFO]: WiFi connected, ready to sync time");

  configTime(0, 0, ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("\n[ERROR]: Failed to get NTP time – leaving timestamp as is");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return;
  }

  timestamp = mktime(&timeinfo);
  lastUpdate = millis();
  Serial.printf("\n[INFO]: NTP sync timestamp: %d", timestamp);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  setupRing();

  preferences.begin("app", true);  //read only
  uint8_t brightness = preferences.getUChar("brightness", 10);
  ssid = preferences.getString("ssid", "YOUR-SSID");
  password = preferences.getString("password", "YOUR-PASSWORD");
  timeMode = static_cast<TimeMode>(preferences.getUChar("time_mode", TimeMode::MANUAL));
  takeAlarmsOut();
  preferences.end();

  Serial.printf("\n[INFO]: System run at %s mode", timeMode == TimeMode::AUTO ? "AUTO" : "MANUAL");

  if (timeMode == TimeMode::AUTO) getTime();

  // Init default value
  appState = State::CLOCK;
  screen = new Clock(timestamp, offset);
  bleScreen = new Ble(new CustomBLECharacteristicCallbacks());

  // Init queue
  frameQueue = xQueueCreate(4, sizeof(CRGB *));
  buttonQueue = xQueueCreate(10, sizeof(uint8_t));

  FastLED.addLeds<LED_TYPE, LED_PIN>(frame, NUM_LEDS);
  FastLED.setBrightness(brightness);

  // Cấu hình các nút (attach Interrupt)
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonInfos[i].pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(buttonInfos[i].pin), buttonISRHandler, &buttonInfos[i], FALLING);
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
        case State::CLOCK:
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
        case State::MENU:
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
        case State::CUSTOM:
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
        case State::GAME:
          screen->onButton(btn, callback);
          break;
        case State::CLOCK_SETTING:
          screen->onButton(btn, callback);
          break;
        case State::BRIGHTNESS:
          switch (btn) {
            case BUTTON_BACK:
              delete screen;
              screen = new Menu();
              appState = MENU;
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
        case State::BLE:
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

  Serial.printf("\n[INFO]: State: %d", appState);

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
  static bool alarmTriggered[5] = { false };  // Track which alarms have triggered to avoid repeated calls

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

    uint8_t hour = ((timestamp + (offset * 3600)) % 86400) / 3600;
    uint8_t minute = (timestamp % 3600) / 60;

    for (int i = 0; i < 5; i += 1) {
      if (alarms[i].hour == -1 && alarms[i].minute == -1) {
        alarmTriggered[i] = false;  // Reset trigger state for unset alarms
        continue;
      }

      if (alarms[i].hour == hour && alarms[i].minute == minute && !alarmTriggered[i]) {
        Serial.printf("\n[INFO]: Alarm %d triggered at %02d:%02d",
              i + 1, alarms[i].hour, alarms[i].minute);
        ring();                    // Call the ring function
        alarmTriggered[i] = true;  // Mark alarm as triggered
      } else if (alarms[i].hour != hour || alarms[i].minute != minute) {
        alarmTriggered[i] = false;  // Reset trigger state when time no longer matches
      }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
