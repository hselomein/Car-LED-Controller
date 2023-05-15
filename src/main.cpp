#define ESP32
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 64
#define REFRESH_RATE 1000
#define PxMATRIX_COLOR_DEPTH 8

//#define BRIGHTNESS 1
int Brightness = 255;
#define BrightFactor (255 / Brightness)

#ifdef ESP32

//#include <PxMatrix.h>
#include <logos.h>
#include <Arduino.h>

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
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);

void drawImage(const bool *image, int width, int height, int xPos, int yPos, COLOR colorF, COLOR colorB) {
  for (int x = 0; x < width; x++ ) {
    for (int y = 0; y < height; y++ ) {
      long Pos = (x + y * width) * 4;
      COLOR col = image[Pos] ? colorF : colorB;
      display.drawPixelRGB888(x + xPos, y + yPos, col.Red * BrightFactor, col.Green * BrightFactor, col.Blue * BrightFactor);
      //display.drawPixelRGB888(x + xPos, y + yPos, 255, 255, 255);
      //display.drawPixel(x + xPos, y + yPos, display.color565(255, 255, 255));
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  display.begin(32);
  display.setDriverChip(FM6124);
  display.setFastUpdate(false);
  display.clearDisplay();
  //display.setBrightness(BRIGHTNESS);
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("CYTECH");
  display.setTextColor(myYELLOW);
  display.setCursor(2,8);
  display.print("Enterprises");
  display.setTextColor(myMAGENTA);
  display.setCursor(2,32);
  display.print("ABCDEHF1234567890!@#$%^&*()-+");
  delay(1000);

  #ifdef ESP32
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 1000, true);
    timerAlarmEnable(timer);
  #endif

  delay(4000);
}


void loop() {
  while (true) {
    Brightness -= 254;
    if (Brightness < 0) {Brightness = 255;}

    display.clearDisplay();
    drawImage(LYFT_LOGO, LYFT_WIDTH, LYFT_HEIGHT, 0, 0, LYFT_COLOR_F, LYFT_COLOR_B);
    delay(1000);

   // display.clearDisplay();
   // drawImage(LYFT_LOGO, LYFT_WIDTH, LYFT_HEIGHT, 0, 0, LYFT_COLOR_F_DIM, LYFT_COLOR_B);
   // delay(1000);

    display.clearDisplay();
    drawImage(UBER_LOGO, UBER_WIDTH, UBER_HEIGHT, 0, 0, UBER_COLOR_F, UBER_COLOR_B);
    delay(1000);
  }
}

