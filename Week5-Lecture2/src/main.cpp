#include <Arduino.h>

#define LED_PIN     18
#define BUZZER_PIN  27

#define LED_PWM_CH      0
#define BUZZER_PWM_CH   1

#define LED_FREQ    5000
#define LED_RES     8

#define BUZZER_FREQ 2000
#define BUZZER_RES  10

void setup() {
  // Setup LED PWM channel
  ledcSetup(LED_PWM_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED_PIN, LED_PWM_CH);

  // Setup Buzzer PWM channel
  ledcSetup(BUZZER_PWM_CH, BUZZER_FREQ, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CH);
}

void loop() {
  // --- LED brightness fade (non-blocking approach) ---
  static int brightness = 0;
  static int fadeAmount = 1;
  
  ledcWrite(LED_PWM_CH, brightness);
  brightness += fadeAmount;
  
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;  // Reverse fade direction
  }
  
  // --- Buzzer beep pattern ---
  static unsigned long lastMillis = 0;
  static int state = 0;
  static int beepCount = 0;

  unsigned long now = millis();

  switch(state) {
    case 0: // Beep ON
      if (beepCount < 3) {
        ledcWriteTone(BUZZER_PWM_CH, 2000 + beepCount * 400);
        if (now - lastMillis > 150) {
          ledcWriteTone(BUZZER_PWM_CH, 0);
          lastMillis = now;
          state = 1;
          beepCount++;
        }
      } else {
        beepCount = 0;
        state = 2; // Move to frequency sweep
        lastMillis = now;
      }
      break;

    case 1: // Beep OFF
      if (now - lastMillis > 150) {
        lastMillis = now;
        state = 0;
      }
      break;

    case 2: // Frequency sweep 400Hz → 3000Hz
      static int freq = 400;
      if (freq <= 3000) {
        ledcWriteTone(BUZZER_PWM_CH, freq);
        if (now - lastMillis > 20) {
          freq += 100;
          lastMillis = now;
        }
      } else {
        ledcWriteTone(BUZZER_PWM_CH, 0);
        freq = 400;
        state = 3;
        lastMillis = now;
      }
      break;

    case 3: // Short melody
      static int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
      static int noteIndex = 0;

      if (noteIndex < 8) {
        ledcWriteTone(BUZZER_PWM_CH, melody[noteIndex]);
        if (now - lastMillis > 250) {
          noteIndex++;
          lastMillis = now;
        }
      } else {
        ledcWriteTone(BUZZER_PWM_CH, 0);
        noteIndex = 0;
        state = 0; // Restart pattern
      }
      break;
  }

  delay(10);  // Smooth LED fading timing
}
