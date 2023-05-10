#include <PxMatrix.h>
#include <logos.h>

#define ESP32
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 64
#define REFRESH_RATE 70
#define BRIGHTNESS 255

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

// Pins for LED MATRIX
PxMATRIX display(DISPLAY_WIDTH,DISPLAY_HEIGHT,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(REFRESH_RATE);
  //display.displayTestPattern(REFRESH_RATE);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


uint16_t myCYAN = display.color565(0, 255, 255);
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  display.begin(32);
  display.setDriverChip(FM6126A);
  display.setBrightness(BRIGHTNESS);
  display.flushDisplay();
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("Pixel");
  Serial.println("hello");
  delay(1000);

  #ifdef ESP32
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  #endif

  delay(3000);
}


void loop() {
  display.clearDisplay();
  drawImage(LYFT_LOGO, LYFT_WIDTH, LYFT_HEIGHT, 0, 0, LYFT_COLOR_F, LYFT_COLOR_B);
  delay(2000);
  display.clearDisplay();
  drawImage(UBER_LOGO, UBER_WIDTH, UBER_HEIGHT, 0, 0, UBER_COLOR_F, UBER_COLOR_B);
  delay(2000);
}

void drawImage(const bool *image, int width, int height, int xPos, int yPos, COLOR colorF, COLOR colorB) {
  for (int x = 0; x < width; x++ ) {
    for (int y = 0; y < height; y++ ) {
      long Pos = (x + y * width) * 4;
      COLOR col = image[Pos] ? colorF : colorB;
      display.drawPixelRGB888(x + xPos, y + yPos, col.Red, col.Green, col.Blue);
      //display.drawPixelRGB888(x + xPos, y + yPos, 255, 255, 255);
      //display.drawPixel(x + xPos, y + yPos, display.color565(255, 255, 255));
    }
  }
}