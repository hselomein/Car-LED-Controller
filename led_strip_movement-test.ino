// Please provide more info here!
/* Use this file to test led strip actions and bahaviors
// This file is just the startup sequence for the car_led_controller
*/
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#include <FastLED.h>

#define LED_PIN         11
#define LED_TYPE        WS2811
#define COLOR_ORDER     BRG

#define NUM_LEDS        30
#define NUM_LEDS_HALF   NUM_LEDS / 2
#define NUM_LEDS_ODD    NUM_LEDS % 2

// NOTE: Brightness should not be altered.
//  Always set this to max brightness.
//  To change Brightness instead alter color value.
#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  128
#define MED_BRIGHTNESS  196

#define COLOR_TEMP UncorrectedTemperature

CRGBArray<NUM_LEDS> leds;


// Code to run once on boot
void setup() {
  delay( 2000 ); // power-up safety delay
  
  // Start LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.setTemperature(COLOR_TEMP);
  
  // Initiate startup lighting sequence
  startup();
}

// Code to repeat
void loop() {
  // Loop code goes here
} 


// Yves contribution <--- Please remove this comment

void startup() {
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
  FastLED.delay(1000);                              //<--- May not be needed
}

void flashLED (int ledLeft, int ledRight, CRGB curColor, int msDelay) {
  leds[ledLeft] = curColor;   leds[ledRight] = curColor;
  FastLED.show();
  FastLED.delay(msDelay);
}    

void ledWave(CRGB maxColor, CRGB minColor, int msDelay, bool boolDirection) {
  // Debug Info:  
    Serial.print(F("Max Color: "));     Serial.println(maxColor);
    Serial.print(F("Min Color: "));     Serial.println(minColor);
    Serial.print(F("Delay: "));         Serial.println(msDelay);
    if (boolDirection) {
      Serial.println(F("Direction: Out"));
    } else {
      Serial.println(F("Direction: In"));
    }

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

    // Debug Info
      Serial.print(F("cur. left: "));     Serial.println(ledLeft);
      Serial.print(F("cur. right: "));    Serial.println(ledRight);

    flashLED (ledLeft, ledRight, maxColor, msDelay);
    flashLED (ledLeft, ledRight, minColor, msDelay / 5);
  }

  // Flash the center LED if number is ODD and direction is IN
  if (!boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, msDelay / 5);
  }
}
