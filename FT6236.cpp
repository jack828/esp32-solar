#include "FT6236.h"

int readTouchRegister(int reg) {
  int data = 0;
  Wire.beginTransmission(TOUCH_I2C_ADD);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(TOUCH_I2C_ADD, 1);
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}

/*
int getTouchPointX()
{
    int XL = 0;
    int XH = 0;

    XH = readTouchRegister(TOUCH_REG_XH);
    XL = readTouchRegister(TOUCH_REG_XL);

    return ((XH & 0x0F) << 8) | XL;
}
*/

int getTouchPointX() {
  int XL = 0;
  int XH = 0;

  XH = readTouchRegister(TOUCH_REG_XH);
  // Serial.println(XH >> 6,HEX);
  if (XH >> 6 == 1)
    return -1;
  XL = readTouchRegister(TOUCH_REG_XL);

  return ((XH & 0x0F) << 8) | XL;
}

int getTouchPointY() {
  int YL = 0;
  int YH = 0;

  YH = readTouchRegister(TOUCH_REG_YH);
  YL = readTouchRegister(TOUCH_REG_YL);

  return ((YH & 0x0F) << 8) | YL;
}

void ft6236_pos(int pos[2]) {
  int XL = 0;
  int XH = 0;
  int YL = 0;
  int YH = 0;

  XH = readTouchRegister(TOUCH_REG_XH);
  if (XH >> 6 == 1) {
    pos[0] = -1;
    pos[1] = -1;
    return;
  }
  XL = readTouchRegister(TOUCH_REG_XL);
  YH = readTouchRegister(TOUCH_REG_YH);
  YL = readTouchRegister(TOUCH_REG_YL);

  // your rotation is important here, amend accordingly
  pos[0] = ((YH & 0x0F) << 8) | YL;
  pos[1] = ((XH & 0x0F) << 8) | XL;
}
