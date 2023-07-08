#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

#include "esp_log.h"

#define MAIN_TAG "main"

#define PIN_SERVO 27
// #define PIN_OLED_SDA 21
// #define PIN_OLED_SDA 22
#define TOUCH_UP T5 // 12
#define TOUCH_DOWN T6 // 14

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define CHECK_INTERVAL_SERVO_MS 500
#define CHECK_INTERVAL_DISPLAY_MS 1000

#define DEGREES_MAX 180
#define DEGREES_MIN 0

Servo servo;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int servoDegrees{};
int lastServoDegrees{};

void setup() {
  ESP_LOGI(MAIN_TAG, "Connecting OLED...");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    ESP_LOGE(MAIN_TAG, "SSD1306 allocation failed");
    for (;;)
      ;
  }
  ESP_LOGI(MAIN_TAG, "OLED is Connected.");

  display.clearDisplay();
  display.setTextColor(WHITE);  // Fixed

  servo.attach(PIN_SERVO);
  servo.write(servoDegrees);
  lastServoDegrees = servoDegrees;
}

unsigned long lastServoChecked{};
unsigned long lastDisplayChecked{};
void loop() {
  int touchUp = touchRead(TOUCH_UP);
  ESP_LOGV(MAIN_TAG, "Up : %d", touchUp);
  if (touchUp < 50) {
    servoDegrees = min(DEGREES_MAX, servoDegrees + 1);
  }
  int touchDown = touchRead(TOUCH_DOWN);
  ESP_LOGV(MAIN_TAG, "Down : %d", touchDown);
  if (touchDown < 50) {
    servoDegrees = max(DEGREES_MIN, servoDegrees - 1);
  }

  unsigned long now = millis();
  if ((lastServoDegrees != servoDegrees) &&
      (now - lastServoChecked > CHECK_INTERVAL_SERVO_MS)) {
    servo.write(servoDegrees);
    ESP_LOGI(MAIN_TAG, "Degrees : %d", servoDegrees);
    lastServoDegrees = servoDegrees;
    lastServoChecked = now;
  }

  if (now - lastDisplayChecked > CHECK_INTERVAL_DISPLAY_MS) {
    display.clearDisplay();

    // display temperature
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Degrees :");

    display.setTextSize(2);
    display.setCursor(0, 10);
    if (servoDegrees < 100) {
      display.print(" ");
      if (servoDegrees < 10) {
        display.print(" ");
      }
    }
    display.print(String(servoDegrees));
    // https://en.wikipedia.org/wiki/Code_page_437
    display.setTextSize(1);
    display.cp437(true);
    display.write(0xA7);

    display.display();

    lastDisplayChecked = now;
  }

  delay(50);
}
