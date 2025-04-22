#ifndef RING_H
#define RING_H

#include <Arduino.h> 

#ifndef BUZZER_PIN_1
  #define BUZZER_PIN_1 6
#endif
#ifndef BUZZER_PIN_2
  #define BUZZER_PIN_2 7
#endif

#define NOTE_C6  1047
#define NOTE_D6  1175
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_G6  1568
#define NOTE_A6  1760
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_E7  2637
#define NOTE_REST 0

typedef struct {
  int note;
  int duration;
  int volume;
} Note;

void playNote(int freq, int duration, int volume);
void setupRing();
void ring();

#endif