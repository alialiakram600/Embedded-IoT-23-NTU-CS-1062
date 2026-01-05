#define BLYNK_TEMPLATE_ID "TMPL6rEL0a3qB"
#define BLYNK_TEMPLATE_NAME "Rider Location"
#define BLYNK_AUTH_TOKEN "ZkzCMs34aMEbpnhjHVGob_7HywbMW3fq"
#define BLYNK_PRINT Serial


#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <math.h>

// ---------------- Blynk Credentials ----------------

// ---------------- OLED Settings ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- MPU6050 Settings ----------------
Adafruit_MPU6050 mpu;
#define BUZZER 14
float THRESHOLD = 25.0;
bool crashSent = false;

// ---------------- WiFi ----------------
char ssid[] = "Oppo";
char pass[] = "1234567890";

// ---------------- Bluetooth ----------------
BluetoothSerial SerialBT;

// ---------------- Blynk Button (V1) ----------------
BLYNK_WRITE(V1) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    Serial.println("Location requested from Blynk");
    SerialBT.println("GET_GPS");

    // OLED feedback
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Blynk Button");
    display.println("GET_GPS sent");
    display.display();
  }
}

void setup() {
  Serial.begin(115200);

  // ---------------- OLED Setup ----------------
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  // ---------------- MPU6050 Setup ----------------
  if (!mpu.begin()) {
    Serial.println("MPU6050 failed");
    display.println("MPU FAIL");
    display.display();
    while (1);
  }

  // ---------------- Bluetooth Setup ----------------
  SerialBT.begin("Rider Location");
  Serial.println("Bluetooth Started");
  display.println("Bluetooth OK");
  display.display();

  // ---------------- WiFi & Blynk ----------------
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connected to Blynk");
  display.println("Blynk Connected");
  display.display();

  // ---------------- Buzzer ----------------
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  Blynk.run();

  // ---------------- MPU6050 Read ----------------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float totalAcc = sqrt(
      a.acceleration.x * a.acceleration.x +
      a.acceleration.y * a.acceleration.y +
      a.acceleration.z * a.acceleration.z
  );

  // Display Acceleration on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Total Acc: ");
  display.println(totalAcc);
  display.display();

  // Accident Detection
  if (totalAcc > THRESHOLD && !crashSent) {
    crashSent = true;

    Serial.println("ACCIDENT!");
    SerialBT.println("crash");   // send crash to MIT App

    digitalWrite(BUZZER, HIGH);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("CRASH!");
    display.println("Message Sent");
    display.display();

    delay(2000);
  }

  // Reset after impact
  if (totalAcc < 15.0) {
    crashSent = false;
    digitalWrite(BUZZER, LOW);
  }

  delay(200);
}
