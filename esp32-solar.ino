#include "credentials.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "planets.h"
#include <math.h>
#include <time.h>

WiFiMulti wifiMulti;
WiFiUDP ntpUDP;
// TODO timezone
NTPClient timeClient(ntpUDP, "uk.pool.ntp.org", 0, 60000);

TFT_eSPI tft = TFT_eSPI();

#define SUN_X 240
#define SUN_Y 160
#define SUN_R 7
#define HEIGHT 320

int colours[NUM_PLANETS] = {TFT_SILVER, TFT_BROWN, TFT_GREEN, TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_SKYBLUE, TFT_NAVY};

void setup() {
  Serial.begin(115200);

  initTft();
  initWifi();
  initNtp();

  tft.fillScreen(TFT_BLACK);
}

int count = 0;
int lastUpdate = 0;
void loop() {
  tft.setCursor(0, 0, 2);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.print("IP: ");
  tft.print(WiFi.localIP());
  tft.print(" Time: ");
  tft.print(timeClient.getFormattedTime());
  tft.print(" Epoch: ");
  tft.println(timeClient.getEpochTime());

  tft.print("count: ");
  tft.print(count++);

  tft.print(" FPS: ");
  tft.println((1 * 1000) / (millis() - lastUpdate));
  lastUpdate = millis();
  tft.setCursor(0, tft.height() - 16, 2);
  tft.print("Free heap: ");
  tft.print(ESP.getFreeHeap());
  tft.print(' ');

  /* GET PLANET POSITIONS */

  tft.fillCircle(SUN_X, SUN_Y, SUN_R, TFT_YELLOW);
  /* tft.drawPixel(SUN_X, SUN_Y, TFT_BLACK); */

  time_t time = timeClient.getEpochTime() + (count * 24 * 60 * 60);
  const tm* timeTm = localtime(&time);
  tft.println(asctime(timeTm));
  Position* planets = coordinates(timeTm->tm_year + 1900, timeTm->tm_mon + 1,
                                  timeTm->tm_mday, timeTm->tm_hour, timeTm->tm_min);

  /* DRAW PLANETS */
  for (int planetIndex = 0; planetIndex < NUM_PLANETS; planetIndex++) {
    Position planet = planets[planetIndex];
      // Radius of planet's orbit
      int r = (SUN_R * 2.5) * (planetIndex + 1) + 2;

      tft.drawCircle(SUN_X, SUN_Y, r, TFT_WHITE);
      double theta = atan2(planet.xeclip, planet.yeclip);
      // calculate angular position
      double planetX = r * sin(theta);
      double planetY = r * cos(theta);

      // adjust relative to CENTRE
      planetX = planetX + SUN_X;
      /* planetY =  (planetY + SUN_Y); */
      planetY = HEIGHT - (planetY + SUN_Y);
      // Draw slightly larger circle to remove previous pixel
      tft.fillCircle((int)planetX, (int)planetY, 7, TFT_BLACK);
      // Draw planet
      tft.fillCircle((int)planetX, (int)planetY, 5, colours[planetIndex]);
      // Draw?
     /* for ar in range(0, len(planetsLib.planetConfigs[planetIndex]), 5):
          planetConfig = planetsLib.planetConfigs[planetIndex]
          print(planet[3], planetConfig)
          // -50 might be because a byte array can't contain
          // negative numbers? But why 50?
          x = planetConfig[ar] - 50 + planetX
          y = planetConfig[ar + 1] - 50 + planetY
          if x >= 0 and y >= 0:
              redValue = planetConfig[ar + 2]
              greenValue = planetConfig[ar + 3]
              blueValue = planetConfig[ar + 4]*/
  }
  free(planets);
  /* delay(1000); */
}

void initTft() {
  tft.init();

  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
}

void initWifi() {
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  Serial.print(F("[ WIFI ] Connecting..."));
  tft.print(F("[ WIFI ] Connecting..."));

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID_1, WIFI_PSK_1);
  wifiMulti.addAP(WIFI_SSID_2, WIFI_PSK_2);
  wifiMulti.addAP(WIFI_SSID_3, WIFI_PSK_3);

  int retryCount = 0;
  // TODO this loop seems to never run...
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
  tft.println(F("done!"));
  Serial.print(F("\n[ WIFI ] connected, SSID: "));
  Serial.print(WiFi.SSID());
  Serial.print(F(", IP:"));
  Serial.println(WiFi.localIP());
  Serial.println();
}

void initNtp() {
  Serial.print(F("[ NTP ] Obtaining time..."));
  tft.print(F("[ NTP ] Obtaining time..."));
  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.print("[ NTP ] time: ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("[ NTP ] epoch: ");
  Serial.println(timeClient.getEpochTime());
}
