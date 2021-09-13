#include "FT6236.h"
#include "credentials.h"
#include "definitions.h"
#include "images.h"
#include "planets.h"
#include <NTPClient.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <math.h>
#include <time.h>

WiFiMulti wifiMulti;
WiFiUDP ntpUDP;
// TODO timezone
NTPClient timeClient(ntpUDP, "uk.pool.ntp.org", 0, 60000);

TFT_eSPI tft = TFT_eSPI();
TFT_eSPI_Button btn;

#define SUN_X 240
#define SUN_Y 160
#define SUN_R 7
#define HEIGHT 320

int colours[NUM_PLANETS] = {TFT_SILVER, TFT_BROWN,  TFT_GREEN,   TFT_RED,
                            TFT_ORANGE, TFT_YELLOW, TFT_SKYBLUE, TFT_NAVY};

// Function definitions for LSP
void initTft(void);
void initTouch(void);
void initWifi(void);
void initNtp(void);

void setup() {
  Serial.begin(115200);

  initTft();
  initTouch();
  initWifi();
  initNtp();

  tft.fillScreen(TFT_BLACK);

  // Sun is static & screen buffer does not reset each frame
  tft.setSwapBytes(!tft.getSwapBytes());
  tft.pushImage(SUN_X - (SUN_WIDTH / 2), SUN_Y - (SUN_HEIGHT / 2), SUN_WIDTH,
                SUN_HEIGHT, (uint16_t *)sunImage);

  btn.initButtonUL(&tft, 20, 100, 50, 50, TFT_WHITE, TFT_WHITE, TFT_BLACK,
                   "PLAY", 1);

  btn.drawButton();
}

int count = 0;
int32_t lastUpdate = 0;
bool paused = true;
Position *planets = (Position *)malloc(NUM_PLANETS * sizeof(Position));

void loop() {
  tft.setCursor(0, 30, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  lastUpdate = millis();
  int pos[2] = {0, 0};
  ft6236_pos(pos);
  tft.print("X: ");
  tft.print(pos[0]);
  tft.print(" Y: ");
  tft.print(pos[1]);
  tft.print(" BTN: ");

  if (pos[0] != -1 && pos[1] != -1 &&
      btn.contains(tft.width() - pos[0], pos[1])) {
    btn.press(true); // tell the button it is pressed
  } else {
    btn.press(false); // tell the button it is NOT pressed
  }
  tft.print(btn.contains(tft.width() - pos[0], pos[1]) ? 'Y' : 'N');
  tft.println("    ");

  if (btn.justReleased()) {
    btn.drawButton(false, paused ? "PLAY" : "PAUSE");
  } else if (btn.justPressed()) {
    paused = !paused;
    btn.drawButton(true, paused ? "PLAY" : "PAUSE");
    Serial.println("PRESSED BOI");
  }

  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // We can now plot text on screen using the "print" class
  tft.print(F("IP: "));
  tft.print(WiFi.localIP());
  tft.print(F(" Time: "));
  tft.print(timeClient.getFormattedTime());
  tft.print(F(" Epoch: "));
  tft.println(timeClient.getEpochTime());

  tft.print(F("count: "));
  tft.print(count);

  tft.print(F(" FPS: "));
  tft.println((1 * 1000) / (millis() - lastUpdate));
  lastUpdate = millis();
  tft.setCursor(0, tft.height() - 16, 2);
  tft.print(F("Free heap: "));
  tft.print(ESP.getFreeHeap());
  tft.print(' ');

  /* GET PLANET POSITIONS */

  // re-render in initial starting state OR if not paused
  if (count == 0 || !paused) {
    uint32_t start = millis();
    time_t time = timeClient.getEpochTime() + (count++ * 24 * 60 * 60);
    const tm *timeTm = localtime(&time);
    tft.print(asctime(timeTm));
    tft.print("    ");
    coordinates(planets, timeTm);
    /* Time To Calculate
    tft.setCursor(0, 80, 2);
    tft.print("  TTC: ");
    tft.print(millis() - start);
    */

    start = millis();
    /* DRAW PLANETS */
    for (int planetIndex = 0; planetIndex < NUM_PLANETS; planetIndex++) {
      Position planet = planets[planetIndex];
      // Radius of planet's orbital position
      int r = (SUN_R * 2.5) * (planetIndex + 1) + 2;

      // Draw circular orbit
      tft.drawCircle(SUN_X, SUN_Y, r, TFT_WHITE);

      // Calculate angle of planet to sun at (0, 0)
      double theta = atan2(planet.xeclip, planet.yeclip);
      // Map to circular orbit position
      double planetX = r * sin(theta);
      double planetY = r * cos(theta);

      // adjust relative to location of sun
      planetX = planetX + SUN_X;
      planetY = HEIGHT - (planetY + SUN_Y);

      // Draw slightly larger circle to remove previous pixel
      tft.fillCircle((int)planetX, (int)planetY, 8, TFT_BLACK);
      // Draw planet image
      tft.pushImage(planetX - (PLANET_WIDTH / 2),
                    planetY - (PLANET_HEIGHT / 2), PLANET_WIDTH, PLANET_HEIGHT,
                    (uint16_t *)planetImages[planetIndex]);
    }
    /* Time To Draw
    tft.print("  TTD: ");
    tft.print(millis() - start);
    */
  }
}

void initTft() {
  tft.init();

  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
}

void initTouch() {
  byte error;
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(TOUCH_I2C_ADD);
  error = Wire.endTransmission();
  if (error == 0) {
    Serial.print("I2C device found at address 0x");
    Serial.print(TOUCH_I2C_ADD, HEX);
    Serial.println("  !");
  } else {
    Serial.print("Unknown error at address 0x");
    Serial.println(TOUCH_I2C_ADD, HEX);
    Serial.print(" - ");
    Serial.println(error);
  }
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

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println(
        F("\n[ WIFI ] ERROR: Could not connect to wifi, rebooting..."));
    Serial.flush();
    ESP.restart();
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
  Serial.print(F("[ NTP ] time: "));
  Serial.println(timeClient.getFormattedTime());
  Serial.print(F("[ NTP ] epoch: "));
  Serial.println(timeClient.getEpochTime());
}
