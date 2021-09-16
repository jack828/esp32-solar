# esp32-solar

## What's it do?

This project was another bordom-fighting exercise. It shows the positions of the planets at a particular date and time.

It also lets you "play" the animation through at about ~30 FPS (which ticks at a rate of 1 day per frame).

The maths used in this project is from [Paul Schlyter](https://stjarnhimlen.se/comp/tutorial.html), and I encourage you to read it as he explains it very well.

![board image](https://user-images.githubusercontent.com/6363719/133695314-29d2a962-7d25-4262-a45c-797d4be26b19.png)

## The Board

Using [Makerfabs' ESP32 Touch with Camera (capacitive)](https://www.makerfabs.com/esp32-3.5-inch-tft-touch-capacitive-with-camera.html)

Which has:
  1. 3.5inch TFT ILI9488
  2. SD card Reader
  3. I2C Touch Screen (NS2009 or FT6236)
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

Sprites generated using [image2cpp](https://jackburgess.dev/tools/image2cpp)

## Author

[Jack Burgess](https://jackburgess.dev)

## License

GNU GPLv3
