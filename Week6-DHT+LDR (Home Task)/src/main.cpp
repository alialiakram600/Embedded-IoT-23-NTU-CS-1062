#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin configuration ---
#define DHTPIN 14        // DHT22 data pin
#define DHTTYPE DHT22    // DHT type: DHT11 or DHT22

#define LDR_PIN 34       // LDR analog pin (for light intensity)
#define SDA_PIN 21       // I2C SDA
#define SCL_PIN 22       // I2C SCL

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT sensor setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Setup function ---
void setup() {
  Serial.begin(115200);
  Serial.println("Starting IoT Project...");

  // Initialize I2C on custom pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  // Initialize DHT sensor
  dht.begin();
  delay(1000);
}

// --- Main loop ---
void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if read failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT22 sensor!");
    return;
  }

  // Read LDR sensor (Light Intensity)
  int ldrValue = analogRead(LDR_PIN);
  float ldrVoltage = (ldrValue / 4095.0) * 3.3;  // Convert to voltage

  // Display data on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(humidity);
  Serial.print(" %  |  LDR: ");
  Serial.print(ldrValue);
  Serial.print("  |  Voltage: ");
  Serial.printf("%.2f V\n", ldrVoltage);

  // Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("IoT Project");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");
  display.setCursor(0, 32);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");
  display.setCursor(0, 48);
  display.print("LDR: ");
  display.print(ldrValue);
  display.print("   ");
  display.print("Voltage: ");
  display.print(ldrVoltage, 2);
  display.println(" V");
  display.display();

  delay(2000); // update every 2 seconds
}