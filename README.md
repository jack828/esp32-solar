# esp32-solar

## The Board

Using Makerfabs' ESP32 Touch with Camera (capacative)

Which has:
  1. 3.5inch TFT ILI9488 driver
  2. SD card Reader
  3. I2C Touch Screen (NS2009 or Ft6236)
  4. OV2640 Camera

## Prerequisites

Install Bodmer's TFT_eSPI library and:
 - In `User_Setup_Select.h`
   - Comment out ALL setups except `#include <User_Setup.h>`
 - In `User_Setup.h`
   - Change the contents to the one in this repo.
 - In `TFT_eSPI.cpp`
   - Head to the bottom and move `#include <Extensions/Button.cpp>` out of the `#if TOUCH_CS` conditional

Install NTPClient

## Credits

Lots of code and main inspiration from dr-mod's repo! https://github.com/dr-mod/pico-solar-system

## Author

[Jack Burgess](https://jackburgess.dev)

## License

GNU GPLv3
