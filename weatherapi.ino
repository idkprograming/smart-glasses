#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_DC 16
#define OLED_CS 5
#define OLED_RESET 17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// WiFi credentials
const char* ssid = "Beilei";
const char* password = "980097www";

String ApiKey = "26e4a77540934cd9a53135121262803";
String location = "New Jersey";

String url;

void setup() {
  Serial.begin(115200);

  // Fix space issue here (IMPORTANT)
  location.replace(" ", "%20");

  url = "http://api.weatherapi.com/v1/current.json?key=" + ApiKey + "&q=" + location;

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED");
  }

  SPI.begin(18, 19, 23, OLED_CS);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("OLED failed");
    while (true)
      ;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    Serial.println("Requesting:");
    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {

      String payload = http.getString();
      Serial.println(payload);

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temp = doc["current"]["temp_f"];
        const char* condition = doc["current"]["condition"]["text"];
        String time = doc["current"]["temp_f"];

        display.clearDisplay();

        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Time:");
        display.print(doc["current"]["last_updated"].as<String>());

        display.setCursor(0, 15);
        display.print("Temp: ");
        display.print(temp);
        display.print("F");

        display.setCursor(0, 30);
        display.print(condition);

        display.display();

      } else {
        Serial.println("JSON parse failed");
      }

    } else {
      Serial.print("HTTP Error: ");
      Serial.println(httpCode);
    }

    http.end();

  } else {
    Serial.println("WiFi disconnected");
  }

  delay(10000);
}