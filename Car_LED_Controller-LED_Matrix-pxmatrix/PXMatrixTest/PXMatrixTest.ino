#include <PxMatrix.h>
#include <logos.h>

#define ESP32
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 64
#define REFRESH_RATE 70

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

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif
// Pins for LED MATRIX

//PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
PxMATRIX display(DISPLAY_WIDTH,DISPLAY_HEIGHT,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(REFRESH_RATE);
  // display.displayTestPixel(REFRESH_RATE);
  display.clearDisplay();
  display.display(REFRESH_RATE);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.clearDisplay();
  display.display(REFRESH_RATE);
  //display.displayTestPattern(REFRESH_RATE);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


uint16_t myCYAN = display.color565(0, 255, 255);
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  display.begin(4);
  display.flushDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("Pixel");
  Serial.println("hello");
  delay(2000);

  #ifdef ESP8266
    display_ticker.attach(0.004, display_updater);
  #endif

  #ifdef ESP32
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  #endif

  delay(1000);
}


void loop() {
  drawImage(LYFT_LOGO, LYFT_WIDTH, LYFT_HEIGHT, 0, 0, LYFT_COLOR_F, LYFT_COLOR_B);
  delay(2000);
  drawImage(UBER_LOGO, UBER_WIDTH, UBER_HEIGHT, 0, 0, UBER_COLOR_F, UBER_COLOR_B);
  delay(2000);
}

void drawImage(const bool *image, int width, int height, int xPos, int yPos, COLOR colorF, COLOR colorB) {
  for (int x = 0; x < width; x++ ) {
    for (int y = 0; y < height; y++ ) {
      long Pos = (x + y * width) * 4;
      COLOR col = image[Pos] ? colorF : colorB;
      //display.drawPixelRGB888(x + xPos, y + yPos, col.Red, col.Green, col.Blue);
      display.drawPixelRGB888(x + xPos, y + yPos, 255, 255, 255);
    }
  }
}