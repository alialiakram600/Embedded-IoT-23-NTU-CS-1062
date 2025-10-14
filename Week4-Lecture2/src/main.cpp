// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }

// #include <Arduino.h>

// #define LED_PIN 2            // GPIO4 for LED
// hw_timer_t *My_timer = nullptr;

// // ---- Timer ISR ----
// void IRAM_ATTR onTimer() {
//   digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // toggle LED
// }

// // ---- Setup ----
// void setup() {
//   pinMode(LED_PIN, OUTPUT);

//   // timerBegin(timer number 0-3, prescaler, countUp)
//   // 80 MHz / 80 = 1 MHz → tick = 1 µs
//   My_timer = timerBegin(0, 80, true);

//   // attach ISR to timer, edge-triggered
//   timerAttachInterrupt(My_timer, &onTimer, true);

//   // trigger every 1 000 000 µs = 1 s
//   timerAlarmWrite(My_timer, 1000000, true);

//   // enable alarm
//   timerAlarmEnable(My_timer);
// }

// void loop() {
//   // main loop free for other code
// }


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // If it fails, check wiring and address (0x3C/0x3D)
    for (;;);
  }

  display.clearDisplay();
  
}

void loop() {
 

  display.clearDisplay();
  display.drawLine(0, 0, 127, 63, SSD1306_WHITE);
  display.drawLine(0, 63, 127, 0, SSD1306_WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(1, 5);
  display.println("Hello");
  display.display();


  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 26);
  display.println("5th-BSCS-B");
  display.display();
  delay(2000);
}