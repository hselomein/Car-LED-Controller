/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <LiquidCrystal_I2C.h>
#include <FastLED.h>

//LED Controller Section
#define LED_PIN         13        // Pin 13 => LED Controller Signal
#define LED_TYPE        WS2812B
#define NUM_LEDS        60
#define NUM_LEDS_HALF   NUM_LEDS / 2
#define NUM_LEDS_ODD    NUM_LEDS % 2
#define COLOR_ORDER     BRG
#define COLOR_TEMP      UncorrectedTemperature
CRGBArray<NUM_LEDS> leds;

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  95
#define MED_BRIGHTNESS  191

#define ANGRY_COLOR     CRGB(255,69,0)      //Orange
#define DEFAULT_COLOR   CRGB(255,255,255)   //White
#define LYFT_COLOR      CRGB(255,0,255)     //Magenta
#define UBER_COLOR      CRGB(255,165,0)     //Cyan

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

String messageStatic = "Static message";
String messageToScroll = "This is a scrolling message with more than 16 characters";

// Function to scroll text
// The function acepts the following arguments:
// row: row number where the text will be displayed
// message: message to scroll
// delayTime: delay between each character shifting
// lcdColumns: number of columns of your LCD
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

void setup(){
  pinMode(LED_PIN, OUTPUT);
  // Start LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.setTemperature(COLOR_TEMP);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  // Initiate startup lighting sequence
  startupSequence();
}

void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print static message
  lcd.print(messageStatic);
  startupSequence();
  // print scrolling message
  scrollText(1, messageToScroll, 250, lcdColumns);
  // Initiate startup lighting sequence
  startupSequence();
} 

void startupSequence() {
  // Configuration (Constants)
  const int msDELAY = 20;   //Number of ms LED stays on for.
  const int numLOOPS = 4;   //Humber of passes over entire LED strip
  const CRGB brightCOLOR =  CRGB( 255, 255, 255);
  const CRGB dimCOLOR =     CRGB( 96, 96, 96);
  const CRGB offCOLOR =     CRGB( 0, 0, 0);

  // Loop 4 times
  //  1 - Towards center clear trail
  //  2 - Away from center clear trail
  //  3 - Towards center remain partial brightness
  //  4 - Away from center remain full brightness 
  CRGB curDimColor = offCOLOR;
  for (int i = 0; i < numLOOPS; i++){
    switch(i) {
      case 2:
        curDimColor = dimCOLOR;
        break;
      case 3:
        curDimColor = brightCOLOR;
        break;
      default:
        curDimColor = offCOLOR;
    }
    // Perform the LED wave effect
    ledWave(brightCOLOR, curDimColor, msDELAY, i % 2);
  }

  // Turn Solid Color:                      //<--- May not be needed
  fill_solid(leds, NUM_LEDS, brightCOLOR);  //<--- May not be needed
  FastLED.show();                           //<--- May not be needed
  //FastLED.delay(1000);                      //<--- May not be needed
}

void flashLED (int ledLeft, int ledRight, CRGB curColor, int msDelay) {
  leds[ledLeft] = curColor;   leds[ledRight] = curColor;
  FastLED.show();
  FastLED.delay(msDelay);
}    

void ledWave(CRGB maxColor, CRGB minColor, int msDelay, bool boolDirection) {
  // Flash the center LED if number is ODD and direction is OUT
  if (boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, msDelay / 5);
  }

  int ledLeft = 0; int ledRight = 0;
  for (int i = 0; i < NUM_LEDS_HALF; i++) {
    // Set current left and right LEDs based on the direction
    if (boolDirection) { ledLeft = NUM_LEDS_HALF - i; }  //Out
    else { ledLeft = i; }                                //In
    ledRight = NUM_LEDS - ledLeft;

    flashLED (ledLeft, ledRight, maxColor, msDelay);
    flashLED (ledLeft, ledRight, minColor, msDelay / 5);
  }

  // Flash the center LED if number is ODD and direction is IN
  if (!boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, msDelay / 5);
  }
}

float Abs(float val) {
  if (val > 0) { return val; }
  else { return -val; }
}
