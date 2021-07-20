#include "credentials.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PSK);
  wifiMulti.addAP(WIFI_SSID, WIFI_PSK);

  Serial.print(F("[ WIFI ] Connecting"));
  int retryCount = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(F("."));
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
    if (retryCount++ > 20) {
      Serial.println(F("\n[ WIFI ] ERROR: Could not connect to wifi, rebooting..."));
      ESP.restart();
    }
  }
  Serial.print(F("\n[ WIFI ] connected, SSID: "));
  Serial.print(WiFi.SSID());
  Serial.print(F(", IP:"));
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  tft.fillScreen(TFT_BLACK);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.println("Intialised default\n");
  tft.println("White text");

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("Red text");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Green text");

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("Blue text");

  delay(5000);
}

void initTft() {
  tft.init();

  tft.setRotation(3);
}
