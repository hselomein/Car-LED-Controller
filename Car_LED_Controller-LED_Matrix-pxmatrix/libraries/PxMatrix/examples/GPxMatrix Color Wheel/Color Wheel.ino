// colorwheel demo for Adafruit RGBmatrixPanel library.
// Renders a nice circle of hues on our 32x32 RGB LED matrix:
// http://www.adafruit.com/products/607
// 32x32 MATRICES DO NOT WORK WITH ARDUINO UNO or METRO 328.

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#define ESP32
#define ARDUINO_ARCH_ESP32

#include <GPxMatrix.h>


#define P_A    19
#define P_B    23
#define P_C    18
#define P_D    5
#define P_E    15
#define P_CLK  14 
#define P_LAT  22 
#define P_OE   16

//                        R1, G1, B1, R2, G2, B2  (RGB Pins)
//uint8_t listrgbpins[6] = {27, 26, 14, 12, 25, 15};
//GPxMatrix matrix(P_A, P_B, P_C, P_D, P_E, P_CLK, P_LAT, P_OE, false, 128, listrgbpins);

GPxMatrix matrix(P_A, P_B, P_C, P_D, P_E, P_CLK, P_LAT, P_OE, false, 64);

void setup() {
  int      x, y, hue;
  float    dx, dy, d;
  uint8_t  sat, val;
  uint16_t c;

  matrix.begin();
  matrix.fillScreen((31 << 11) + (63 << 5) + 31);
  matrix.updateDisplay();
  delay(2000);
  matrix.fillScreen(0);

  for(x=0; x < 64; x++) {
    dx = 31.5 - (float)x;
    for(y=0; y < 64; y++) {
      dy = 31.5 - (float)y;
      hue = (int)((atan2(dx, -dy) + PI) * 180.0 / PI);
      d = sqrt(dx * dx + dy * dy);
      if (d <= 31.5) {
        sat = (int)(d / 31.5 * 255.0 + 0.5);
        val = 255;
      } else {
        sat = 0;
        val = (int)(255.5 - (d - 31.5) * 18);
    }      
    c = matrix.ColorHSV(hue, sat, val, true);
    matrix.drawPixel(x, y, c);
    }
  }
  matrix.updateDisplay();
  delay(5000);
}

void loop() {
  matrix.fillScreen(matrix.Color888(255, 0, 0));
  matrix.updateDisplay();
  delay(1000);
  matrix.fillScreen(matrix.Color888(0, 255, 0));
  matrix.updateDisplay();
  delay(1000);
  matrix.fillScreen(matrix.Color888(0, 0, 255));
  matrix.updateDisplay();
  delay(1000);
}