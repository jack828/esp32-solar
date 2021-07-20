#include "credentials.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiMulti wifiMulti;
WiFiUDP ntpUDP;
// TODO timezone
NTPClient timeClient(ntpUDP, "uk.pool.ntp.org", 0, 60000);

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  initTft();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID_1, WIFI_PSK_1);
  wifiMulti.addAP(WIFI_SSID_2, WIFI_PSK_2);
  wifiMulti.addAP(WIFI_SSID_3, WIFI_PSK_3);

  Serial.print(F("[ WIFI ] Connecting"));
  int retryCount = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(F("."));
    delay(250);
    Serial.print(F("."));
    delay(250);
    Serial.print(F("."));
    delay(250);
    Serial.print(F("."));
    delay(250);
    if (retryCount++ > 20) {
      Serial.println(F("\n[ WIFI ] ERROR: Could not connect to wifi, rebooting..."));
      Serial.flush();
      ESP.restart();
    }
  }
  Serial.print(F("\n[ WIFI ] connected, SSID: "));
  Serial.print(WiFi.SSID());
  Serial.print(F(", IP:"));
  Serial.println(WiFi.localIP());
  Serial.println();

  initNtp();
}

int count = 0;
int lastUpdate = 0;
void loop() {

  // Set "cursor" at top left corner of display (0, 0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.println("Initialised default\n");
  tft.print("IP: ");
  tft.println(WiFi.localIP());

  /* tft.setTextColor(TFT_RED, TFT_BLACK); */
  /* tft.println("Red text"); */
  /* tft.setTextColor(TFT_GREEN, TFT_BLACK); */
  /* tft.println("Green text"); */
  /* tft.setTextColor(TFT_BLUE, TFT_BLACK); */
  /* tft.println("Blue text"); */

  tft.print("Time: ");
  tft.println(timeClient.getFormattedTime());
  tft.print("Epoch: ");
  tft.println(timeClient.getEpochTime());

  tft.print("count: ");
  tft.println(count++);

  tft.print("FPS: ");
  tft.println((1 * 1000) / (millis() - lastUpdate));
  lastUpdate = millis();
  /* delay(1000); */
}

void initTft() {
  tft.init();

  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
}

void initNtp() {
  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.print("[ NTP ] time: ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("[ NTP ] epoch: ");
  Serial.println(timeClient.getEpochTime());
}
