// ///////////////////////       CODE 1 — WiFi Scanner + OLED Display (No Connection)


// #include <WiFi.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// void setup() {
//   Serial.begin(115200);

//   //Start OLED
//   Wire.begin(21, 22);  // SDA, SCL
//   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
//     Serial.println("OLED init failed");
//     while (1);
//   }
//   display.clearDisplay();
//   display.display();

//   //WiFi Scan Mode
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect();
//   delay(1000);
// }

// void loop() {
//   display.clearDisplay();
//   display.setCursor(0, 0);
//   display.setTextColor(SSD1306_WHITE);
//   display.setTextSize(1);

//   display.println("Scanning WiFi...");
//   display.display();

//   Serial.println("Scanning WiFi...");
//   int count = WiFi.scanNetworks();

//   display.clearDisplay();
//   display.setCursor(0, 0);

//   if (count <= 0) {
//     display.println("No networks found");
//     Serial.println("No networks found");
//   } else {
//     display.print("Found: ");
//     display.println(count);
//     Serial.printf("Found %d networks:\n", count);

//     //Show up to 5 networks
//     for (int i = 0; i < count && i < 5; i++) {
//       display.print(i + 1);
//       display.print(": ");
//       display.println(WiFi.SSID(i));

//       Serial.printf("%d: %s (%d dBm)\n",
//                     i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
//     }
//   }

//   display.display();
//   delay(5000);
// }




//////////////////////////////////////         CODE 2 — WiFi Connect + OLED Display

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid     = "DESKTOP-M3MOJCM";
const char* password = "12345678";

void showOLED(String msg) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println(msg);
  display.display();
}

void setup() {
  Serial.begin(115200);

  // OLED Start
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  showOLED("Connecting to WiFi...");
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  String msg = "WiFi Connected\n";
  msg += ssid;
  msg += "\nIP: ";
  msg += WiFi.localIP().toString();

  showOLED(msg);
}

void loop() {
  // Nothing here — stays connected
}
