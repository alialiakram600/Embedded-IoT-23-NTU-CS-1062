// Assignment 01
// Switch Modes and PWD
// Embedded IoT System Fall-2025
// Name: Muhammad Ali Akram        Reg: 23-NTU-CS-1062


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =================== OLED SETUP ===================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// =================== TASK A PINS ===================
#define LED1        19     // LED 1
#define LED2        18     // LED 2
#define BTN_MODE    26     // Button to cycle modes
#define BTN_RESET   0      // Onboard BOOT button (reset to OFF)

// =================== TASK B PINS ===================
#define LED3        17     // Single LED for Task B
#define BTN_EVENT   25     // Button for short/long press
#define BUZZER_PIN  32     // Buzzer pin

// =================== PWM SETTINGS ===================
#define PWM_CH_LED1   0
#define PWM_CH_LED2   1
#define PWM_CH_BUZZER 2
#define PWM_FREQ      5000
#define PWM_RES       10    // 10-bit (0–1023)

// =================== VARIABLES ===================
// ---- Task A ----
int mode = 0; // 0=OFF, 1=Blink, 2=ON, 3=Fade
bool ledState = false;
unsigned long prevMillisBlink = 0;
int fadeVal = 0;
int fadeDir = 10;
unsigned long lastDebounce = 0;
const int debounceDelay = 300;

// ---- Task B ----
bool pressed = false;
unsigned long pressStart = 0;
bool led3State = false;
bool buzzerActive = false;

// =================== OLED FUNCTION ===================
void showOLED(const String &line1, const String &line2 = "") {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 5);
  display.print(line1);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(line2);
  display.display();
}

// =================== SETUP ===================
void setup() {
  Serial.begin(115200);

  // ---- OLED ----
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed!");
    for (;;);
  }
  showOLED("System", "Initializing...");
  delay(1000);

  // ---- LEDs and Buttons ----
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BTN_EVENT, INPUT_PULLUP);

  // ---- PWM Setup ----
  ledcSetup(PWM_CH_LED1, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_LED2, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_BUZZER, PWM_FREQ, PWM_RES);

  ledcAttachPin(LED1, PWM_CH_LED1);
  ledcAttachPin(LED2, PWM_CH_LED2);
  ledcAttachPin(BUZZER_PIN, PWM_CH_BUZZER);

  // ---- Start in Mode 0 (OFF) ----
  ledcWrite(PWM_CH_LED1, 0);
  ledcWrite(PWM_CH_LED2, 0);
  showOLED("Mode 1", "Both OFF");
}

// =================== LOOP ===================
void loop() {
  unsigned long now = millis();

  // ===================== TASK A =====================
  bool btnMode = digitalRead(BTN_MODE);
  bool btnReset = digitalRead(BTN_RESET);

  // ---- Mode Button (Cycle) ----
  if (btnMode == LOW && (now - lastDebounce > debounceDelay)) {
    lastDebounce = now;
    mode = (mode + 1) % 4;
    switch (mode) {
      case 0: showOLED("Mode 1", "Both OFF"); break;
      case 1: showOLED("Mode 2", "Alt Blink"); break;
      case 2: showOLED("Mode 3", "Both ON"); break;
      case 3: showOLED("Mode 4", "PWM Fade"); break;
    }
  }

  // ---- Reset Button (BOOT) ----
  if (btnReset == LOW && (now - lastDebounce > debounceDelay)) {
    lastDebounce = now;
    mode = 0;
    showOLED("Mode 1", "Reset to OFF");
  }

  // ---- LED Control ----
  switch (mode) {
    case 0: // Both OFF
      ledcWrite(PWM_CH_LED1, 0);
      ledcWrite(PWM_CH_LED2, 0);
      break;

    case 1: // Alternate blink
      if (now - prevMillisBlink >= 500) {
        prevMillisBlink = now;
        ledState = !ledState;
        ledcWrite(PWM_CH_LED1, ledState ? 512 : 0);
        ledcWrite(PWM_CH_LED2, ledState ? 0 : 512);
      }
      break;

    case 2: // Both ON
      ledcWrite(PWM_CH_LED1, 512);
      ledcWrite(PWM_CH_LED2, 512);
      break;

    case 3: // PWM fade
      ledcWrite(PWM_CH_LED1, fadeVal);
      ledcWrite(PWM_CH_LED2, fadeVal);
      fadeVal += fadeDir;
      if (fadeVal <= 0 || fadeVal >= 1023) fadeDir = -fadeDir;
      delay(5);
      break;
  }

  // ===================== TASK B =====================
  bool btnEvent = digitalRead(BTN_EVENT);

  if (btnEvent == LOW && !pressed) {
    pressed = true;
    pressStart = now;
    buzzerActive = false;
  }

  if (pressed && btnEvent == LOW) {
    // Button is being held
    unsigned long holdTime = now - pressStart;
    if (holdTime > 1500 && !buzzerActive) {
      // ---- Long press detected (while holding) ----
      showOLED("Long Press", "Buzzer Tone");
      ledcWriteTone(PWM_CH_BUZZER, 1000);
      buzzerActive = true;
      Serial.println("Long press started - buzzer active");
    }
  }

  if (btnEvent == HIGH && pressed) {
    // Button released
    pressed = false;

    if (!buzzerActive) {
      // ---- Short press ----
      led3State = !led3State;
      digitalWrite(LED3, led3State);
      showOLED("ShortPress", led3State ? "LED3 ON" : "LED3 OFF"); // fixed wrapping
      Serial.println("Short press detected.");
    } else {
      // ---- Stop buzzer when released ----
      ledcWrite(PWM_CH_BUZZER, 0);
      buzzerActive = false;
      Serial.println("Long press released - buzzer stopped");
    }
  }
}


// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// // =================== OLED SETUP ===================
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_ADDR 0x3C
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// // =================== TASK A PINS ===================
// #define LED1        19     // LED 1
// #define LED2        18      // LED 2
// #define BTN_MODE    26      // Button to cycle modes
// #define BTN_RESET   0       // Onboard BOOT button (reset to OFF)

// // =================== TASK B PINS ===================
// #define LED3        17      // Single LED for Task B
// #define BTN_EVENT   25      // Button for short/long press
// #define BUZZER_PIN  32     // Buzzer pin

// // =================== PWM SETTINGS ===================
// #define PWM_CH_LED1   0
// #define PWM_CH_LED2   1
// #define PWM_CH_BUZZER 2
// #define PWM_FREQ      5000
// #define PWM_RES       10    // 10-bit (0–1023)

// // =================== VARIABLES ===================
// // ---- Task A ----
// int mode = 0; // 0=OFF, 1=Blink, 2=ON, 3=Fade
// bool ledState = false;
// unsigned long prevMillisBlink = 0;
// int fadeVal = 0;
// int fadeDir = 10;
// unsigned long lastDebounce = 0;
// const int debounceDelay = 300;

// // ---- Task B ----
// bool pressed = false;
// unsigned long pressStart = 0;
// bool led3State = false;

// // =================== OLED FUNCTION ===================
// void showOLED(const String &line1, const String &line2 = "") {
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 5);
//   display.print(line1);
//   display.setTextSize(1);
//   display.setCursor(0, 40);
//   display.print(line2);
//   display.display();
// }

// // =================== SETUP ===================
// void setup() {
//   Serial.begin(115200);

//   // ---- OLED ----
//   Wire.begin(21, 22);
//   if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
//     Serial.println("OLED init failed!");
//     for (;;);
//   }
//   showOLED("System", "Initializing...");
//   delay(1000);

//   // ---- LEDs and Buttons ----
//   pinMode(LED1, OUTPUT);
//   pinMode(LED2, OUTPUT);
//   pinMode(LED3, OUTPUT);
//   pinMode(BTN_MODE, INPUT_PULLUP);
//   pinMode(BTN_RESET, INPUT_PULLUP);
//   pinMode(BTN_EVENT, INPUT_PULLUP);

//   // ---- PWM Setup ----
//   ledcSetup(PWM_CH_LED1, PWM_FREQ, PWM_RES);
//   ledcSetup(PWM_CH_LED2, PWM_FREQ, PWM_RES);
//   ledcSetup(PWM_CH_BUZZER, PWM_FREQ, PWM_RES);

//   ledcAttachPin(LED1, PWM_CH_LED1);
//   ledcAttachPin(LED2, PWM_CH_LED2);
//   ledcAttachPin(BUZZER_PIN, PWM_CH_BUZZER);

//   // ---- Start in Mode 0 (OFF) ----
//   ledcWrite(PWM_CH_LED1, 0);
//   ledcWrite(PWM_CH_LED2, 0);
//   showOLED("Mode 1", "Both OFF");
// }

// // =================== LOOP ===================
// void loop() {
//   unsigned long now = millis();

//   // ===================== TASK A =====================
//   bool btnMode = digitalRead(BTN_MODE);
//   bool btnReset = digitalRead(BTN_RESET);

//   // ---- Mode Button (Cycle) ----
//   if (btnMode == LOW && (now - lastDebounce > debounceDelay)) {
//     lastDebounce = now;
//     mode = (mode + 1) % 4;
//     switch (mode) {
//       case 0: showOLED("Mode 1", "Both OFF"); break;
//       case 1: showOLED("Mode 2", "Alt Blink"); break;
//       case 2: showOLED("Mode 3", "Both ON"); break;
//       case 3: showOLED("Mode 4", "PWM Fade"); break;
//     }
//   }

//   // ---- Reset Button (BOOT) ----
//   if (btnReset == LOW && (now - lastDebounce > debounceDelay)) {
//     lastDebounce = now;
//     mode = 0;
//     showOLED("Mode 1", "Reset to OFF");
//   }

//   // ---- LED Control ----
//   switch (mode) {
//     case 0: // Both OFF
//       ledcWrite(PWM_CH_LED1, 0);
//       ledcWrite(PWM_CH_LED2, 0);
//       break;

//     case 1: // Alternate blink
//       if (now - prevMillisBlink >= 500) {
//         prevMillisBlink = now;
//         ledState = !ledState;
//         ledcWrite(PWM_CH_LED1, ledState ? 512 : 0);
//         ledcWrite(PWM_CH_LED2, ledState ? 0 : 512);
//       }
//       break;

//     case 2: // Both ON
//       ledcWrite(PWM_CH_LED1, 512);
//       ledcWrite(PWM_CH_LED2, 512);
//       break;

//     case 3: // PWM fade
//       ledcWrite(PWM_CH_LED1, fadeVal);
//       ledcWrite(PWM_CH_LED2, fadeVal);
//       fadeVal += fadeDir;
//       if (fadeVal <= 0 || fadeVal >= 1023) fadeDir = -fadeDir;
//       delay(5); // small delay for smooth fade
//       break;
//   }

//   // ===================== TASK B =====================
//   bool btnEvent = digitalRead(BTN_EVENT);

//   if (btnEvent == LOW && !pressed) {
//     pressed = true;
//     pressStart = now;
//   }

//   if (btnEvent == HIGH && pressed) {
//     pressed = false;
//     unsigned long pressDuration = now - pressStart;

//     if (pressDuration < 1500) {
//       // ---- Short press ----
//       led3State = !led3State;
//       digitalWrite(LED3, led3State);
//       showOLED("Short Press", led3State ? "LED3 ON" : "LED3 OFF");
//       Serial.println("Short press detected.");
//     } else {
//       // ---- Long press ----
//       showOLED("Long Press", "Buzzer Tone");
//       ledcWriteTone(PWM_CH_BUZZER, 1000);
//       delay(400);
//       ledcWrite(PWM_CH_BUZZER, 0);
//       Serial.println("Long press detected.");
//     }
//   }
// }
