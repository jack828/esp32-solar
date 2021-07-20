#include "FT6236.h"
#include "definitions.h"
#include <LovyanGFX.hpp>
#include <SPI.h>
#include <Wire.h>

#define TAB_WIDTH 80
#define BEGIN_LEN 10
#define BUTTON_LEN 60
#define BUTTON_HIG 360

struct LGFX_Config {
  static constexpr spi_host_device_t spi_host = LCD_SPI_HOST;
  static constexpr int dma_channel = 1;
  static constexpr int spi_sclk = LCD_SCK;
  static constexpr int spi_mosi = LCD_MOSI;
  static constexpr int spi_miso = LCD_MISO;
};

static lgfx::LGFX_SPI<LGFX_Config> TFT;
static LGFX_Sprite sprite(&TFT);
static lgfx::Panel_ILI9488 panel;

int a[4][4] = {0};
int empty;

void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);
  byte error, address;

  Wire.beginTransmission(TOUCH_I2C_ADD);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.print("I2C device found at address 0x");
    Serial.print(TOUCH_I2C_ADD, HEX);
    Serial.println("  !");
  } else if (error == 4) {
    Serial.print("Unknown error at address 0x");
    Serial.println(TOUCH_I2C_ADD, HEX);
  }

  set_tft();
  TFT.begin();
}

void loop() {

  game_start();
  draw_button();
  delay(100);
}

void game_start() {
  TFT.setTextColor(TFT_WHITE);
  TFT.setTextSize(5);
  // half width - num int * int width in pixels
  TFT.setCursor(0, 100);
  TFT.println("TESTERING");
  TFT.setTextSize(2);
  TFT.setCursor(0, 200);
  TFT.println("ESP32 TOUCH CAMERA");
}

void draw_button() {
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.fillRect(10, BUTTON_HIG, BUTTON_LEN, BUTTON_LEN, TFT_WHITE);
  TFT.setCursor(10, BUTTON_HIG + 20);
  TFT.println("  UP");

  TFT.fillRect(90, BUTTON_HIG, BUTTON_LEN, BUTTON_LEN, TFT_WHITE);
  TFT.setCursor(90, BUTTON_HIG + 20);
  TFT.println("  DOWN");

  TFT.fillRect(170, BUTTON_HIG, BUTTON_LEN, BUTTON_LEN, TFT_WHITE);
  TFT.setCursor(170, BUTTON_HIG + 20);
  TFT.println("  LEFT");

  TFT.fillRect(250, BUTTON_HIG, BUTTON_LEN, BUTTON_LEN, TFT_WHITE);
  TFT.setCursor(250, BUTTON_HIG + 20);
  TFT.println("  RIGHT");
}

void set_tft() {
  // Substitute various setting values for the panel class.
  // (If you select the panel class of the LCD integrated product,
  // No setting is required as the initial value is set according to the
  // product)

  // Set the SPI clock during normal operation.
  // The ESP32 SPI can only be used with 80MHz divided by an integer.
  // The settable value closest to the set value will be used.
  panel.freq_write = 60000000;
  // panel.freq_write = 20000000;

  // Set the SPI clock for single color fill processing.
  // Basically set the same value as freq_write,
  // It may work even if you set a higher value.
  panel.freq_fill = 60000000;
  // panel.freq_fill = 27000000;

  // Set the SPI clock when reading pixel data from the LCD.
  panel.freq_read = 16000000;

  // Set the SPI communication mode from 0 to 3.
  panel.spi_mode = 0;

  // Set the SPI communication mode when reading data from 0 to 3.
  panel.spi_mode_read = 0;

  // Set the number of dummy bits when reading pixels.
  // Adjust if bit shift occurs when reading pixels.
  panel.len_dummy_read_pixel = 8;

  // Set true for panels that can read data, false if not.
  // If omitted, it will be true.
  panel.spi_read = true;

  // Set true for panels that read data with MOSI pins.
  // If omitted, it will be false.
  panel.spi_3wire = false;

  // Set the pin number to which the LCD CS is connected.
  // If not used, omit it or set -1.
  panel.spi_cs = LCD_CS;

  // Set the pin number to which the LCD DC is connected.
  panel.spi_dc = LCD_DC;

  // Set the pin number to which the LCD RST is connected.
  // If not used, omit it or set -1.
  panel.gpio_rst = LCD_RST;

  // Set the pin number to which the LCD backlight is connected.
  // If not used, omit it or set -1.
  panel.gpio_bl = LCD_BL;

  // When using the backlight, set the PWM channel number used for brightness
  // control. If you do not use PWM brightness control, omit it or set it to -1.
  panel.pwm_ch_bl = -1;

  // Set whether the output level when the backlight is on is low or high.
  // Default is true. Lights up when true = HIGH / Lights up when false = LOW.
  panel.backlight_level = true;

  // Set the initial value of invertDisplay. If set to true, it will be
  // inverted. If omitted, false. If the screen color is reversed, change the
  // setting.
  panel.invert = false;

  // Set the color order of the panel. RGB = true / BGR = false
  // Default to false. If red and blue are swapped, change the setting.
  panel.rgb_order = false;

  // Set the number of pixels (width and height) that the panel memory has.
  // If the settings do not match, the coordinates when using setRotation will
  // shift. (Example: ST7735 has 3 types of 132x162 / 128x160 / 132x132)
  panel.memory_width = LCD_WIDTH;
  panel.memory_height = LCD_HEIGHT;

  // Set the actual number of pixels (width and height) of the panel.
  // If omitted, the default value of the panel class is used.
  panel.panel_width = LCD_WIDTH;
  panel.panel_height = LCD_HEIGHT;

  // Set the panel offset amount.
  // If omitted, the default value of the panel class is used.
  panel.offset_x = 0;
  panel.offset_y = 0;

  // Set the value immediately after the initialization of setRotation.
  panel.rotation = 3;

  // Set offset_rotation if you want to change the orientation when using
  // setRotation. Set 1 if you want the orientation in setRotation (0) to be the
  // orientation when it is 1.
  panel.offset_rotation = 0;

  // After completing the settings, pass the panel pointer with the LGFX
  // setPanel function.
  TFT.setPanel(&panel);
}
