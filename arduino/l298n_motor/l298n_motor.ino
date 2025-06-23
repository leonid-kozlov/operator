#include "pitches.h"
#define REST      0

#define SPEAKER_PIN 11
#define BTN1_PIN 2
#define BTN2_PIN 3
#define LED_PIN 13
#define LED_RED_PIN 4
#define LED_GREEN_PIN 5
#define SWITCH_PIN 7
#define RELAY_PIN 11
#define MOTOR_1_PIN_1 8
#define MOTOR_1_PIN_2 9
#define MOTOR_1_PWM 10

int melody[] = {
  // NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
  // NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
  NOTE_A4,4, NOTE_A4,4, NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16,

  NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,//4
  // NOTE_E5,4, NOTE_E5,4, NOTE_E5,4, NOTE_F5,-8, NOTE_C5,16,
  // NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,
  
  // NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
  // NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

  // NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
  // NOTE_C5,4, NOTE_A4,-8, NOTE_C5,16, NOTE_E5,2,

  // NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
  // NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

  // NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
  // NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,
};
// change this to make the song slower or faster
int tempo = 120;
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
void imperial_march() {
  // iterate over the notes of the melody. 
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(SPEAKER_PIN, melody[thisNote], noteDuration*0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    
    // stop the waveform generation before the next note.
    noTone(SPEAKER_PIN);
  }
}


void setup() {
  // imperial_march();

  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(SWITCH_PIN, INPUT);

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);

  pinMode(MOTOR_1_PIN_1, OUTPUT);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void m1_forward() {
  analogWrite(MOTOR_1_PWM, 150);
  digitalWrite(MOTOR_1_PIN_1, HIGH);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void m1_backward() {
  analogWrite(MOTOR_1_PWM, 150);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, HIGH);
}

void m1_stop() {
  analogWrite(MOTOR_1_PWM, 0);
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, LOW);
}

void loop() {
  uint8_t bnt1 = digitalRead(BTN1_PIN);
  uint8_t bnt2 = digitalRead(BTN2_PIN);
  uint8_t sw = digitalRead(SWITCH_PIN);
  if (sw == LOW) {
      m1_stop();
      noTone(SPEAKER_PIN);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, LOW);
      digitalWrite(LED_RED_PIN, LOW);
  } else {
    while (bnt1 or bnt2) {
      bnt1 = digitalRead(BTN1_PIN);
      bnt2 = digitalRead(BTN2_PIN);
      uint8_t led_state = digitalRead(LED_PIN);
      digitalWrite(LED_PIN, !led_state);
      if (bnt1) {
        // digitalWrite(RELAY_PIN, HIGH);
        m1_forward();
        tone(SPEAKER_PIN, NOTE_C4, 40);
        digitalWrite(LED_GREEN_PIN, HIGH);
        digitalWrite(LED_RED_PIN, LOW);
      }
      if (bnt2) {
        // digitalWrite(RELAY_PIN, LOW);
        m1_backward();
        tone(SPEAKER_PIN, NOTE_C7, 40);
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, HIGH);
      }
      delay(50);
    }
    m1_stop();
    noTone(SPEAKER_PIN);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);
  }
  delay(50);
}
