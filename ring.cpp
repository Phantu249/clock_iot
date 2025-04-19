#include <pgmspace.h>
#include "ring.h"

const Note melody[] PROGMEM = {
  { NOTE_G6, 200, 80 }, { NOTE_A6, 200, 100 }, { NOTE_B6, 200, 120 }, { NOTE_C7, 200, 127 },
  { NOTE_B6, 200, 120 }, { NOTE_A6, 200, 100 }, { NOTE_G6, 400, 80 }, { NOTE_REST, 150, 0 },
  
  { NOTE_E6, 200, 80 }, { NOTE_G6, 200, 100 }, { NOTE_A6, 200, 120 }, { NOTE_B6, 200, 127 },
  { NOTE_A6, 200, 120 }, { NOTE_G6, 200, 100 }, { NOTE_E6, 400, 80 }, { NOTE_REST, 150, 0 },
  
  { NOTE_G6, 200, 80 }, { NOTE_A6, 200, 100 }, { NOTE_B6, 200, 120 }, { NOTE_E7, 200, 127 },
  { NOTE_B6, 200, 120 }, { NOTE_A6, 200, 100 }, { NOTE_G6, 400, 80 }, { NOTE_REST, 150, 0 },
};

const int numNotes = sizeof(melody) / sizeof(melody[0]);

void playNote(int freq, int duration, int volume) {
  if (freq == NOTE_REST) {
    ledcWrite(BUZZER_PIN_1, 0);
    ledcWrite(BUZZER_PIN_2, 0);
    vTaskDelay(pdMS_TO_TICKS(duration)); // Chuyển sang vTaskDelay
  } else {
    ledcWriteTone(BUZZER_PIN_1, freq);
    ledcWriteTone(BUZZER_PIN_2, freq);
    ledcWrite(BUZZER_PIN_1, volume);
    ledcWrite(BUZZER_PIN_2, volume);
    vTaskDelay(pdMS_TO_TICKS(duration * 0.9));
    for (int i = volume; i >= 0; i -= 10) {
      ledcWrite(BUZZER_PIN_1, i);
      ledcWrite(BUZZER_PIN_2, i);
      vTaskDelay(pdMS_TO_TICKS(2));
    }
  }
  ledcWrite(BUZZER_PIN_1, 0);
  ledcWrite(BUZZER_PIN_2, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
}

void setupRing() {
  ledcAttach(BUZZER_PIN_1, 10000, 12); // PWM 10kHz, 12-bit
  ledcAttach(BUZZER_PIN_2, 10000, 12);
}

void ring() {
  for (int repeat = 0; repeat < 4; repeat++) {
    for (int i = 0; i < numNotes; i++) {
      // Đọc dữ liệu từ PROGMEM
      int note = pgm_read_word(&melody[i].note);
      int duration = pgm_read_word(&melody[i].duration);
      int volume = pgm_read_word(&melody[i].volume);
      playNote(note, duration, volume);
    }
    delay(400);
  }
}