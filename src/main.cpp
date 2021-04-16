#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config/config.h"
#include "functions/wifi-connection.h"
#include "functions/ota.h"
#include "functions/deep-sleep.h"
#include "functions/temp-bme280.h"
#include "functions/mqtt.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RTC_DATA_ATTR int bootCount = 0;
float temp;
float humidity;
float pressure;
int8_t wifi_strength;

esp_sleep_wakeup_cause_t wakeup_reason;

#define I2C_SDA 21
#define I2C_SCL 22

void updateWiFiSignalStrength() {
  if(WiFi.isConnected()) {
    //serial_println(F("[WIFI] Updating signal strength..."));
    wifi_strength = WiFi.RSSI();
    Serial.print("[WIFI] signal strength: ");
    Serial.println(wifi_strength);
  }
}

void displayStats() {
  // clear display
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // display temperature
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(temp);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("F");
  
  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(humidity);
  display.print(" %");

  // display wifi signal strength
  display.setTextSize(1);
  display.setCursor(110, 56);
  display.print(String(wifi_strength));
  
  display.display();
  delay(1000); // Pause for 1 second
}

void callback(){
  //placeholder callback function
  Serial.println("Touchpad activated, do some work...");
}

void setup() {

  Serial.begin(115200);

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  setup_sleep();
  Wire.begin(I2C_SDA, I2C_SCL);

  setupTemp();
  measureTemp();

  // Display stats waking up from touch sensor
  if (wakeup_reason == 5) {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    displayStats();
  }

  connectWiFi();
  updateWiFiSignalStrength();
  mqttConnect();
  sendTempToMQTT();
  mqttDisconnect();

  // When wakeing up from sensor, update stats to show wifi strength.
  // Then pause long enough for stats to be read.
  if (wakeup_reason == 5) {
    displayStats();
    delay(7000); // Pause for 10 second
    display.clearDisplay();
    display.display();
  }

  // Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);

  // Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();

  // Check OTA update on first boot
  if (bootCount == 1) {
    setupOTA();
    delay(2000);
    // takes a few attempts.  worked on count=11 (2s each)
    unsigned short maxRetry = 20;
    unsigned short countRetry = 0;
    while(countRetry <= maxRetry) {
        Serial.println("[OTA] count: " + String(countRetry));
        countRetry++;
        ArduinoOTA.handle();
        delay(2000);
    }
  }

  deep_sleep();
}

void loop() {
}
