// Please provide more info here!
/* Use this file to test led strip actions and bahaviors*/
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#include <FastLED.h>

#define LED_PIN     11
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG

#define NUM_LEDS    30
#define NUM_LEDS_HALF  NUM_LEDS / 2
#define NUM_LEDS_ODD  NUM_LEDS % 2

// NOTE: Brightness should not be altered.
//  Always set this to max brightness.
//  To change Brightness instead alter color value.
#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  128
#define MED_BRIGHTNESS  196

CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100 //<---Check if this can be increased for better visuals and how do we use it (it is not being used now)

// Not sure if all this is needed
CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Code to run once on boot
void setup() {
  Serial.begin(9600);  
  // Set RelayPin as an output pin
  int RelayPin = 2; 
  pinMode(RelayPin, OUTPUT);
  
  // Start LEDs
  delay( 500 ); // power-up safety delay
  digitalWrite(RelayPin, LOW);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(MAX_BRIGHTNESS);
  
  // Need to explore the lines below
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
  const int msDELAY = 25;
  const CRGB brightCOLOR = 0xFFFFFF;
  const CRGB dimCOLOR = 0xC0C0C0;
  const CRGB offCOLOR = 0x000000;
  const int numLOOPS = 4;

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
  delay(1000);                              //<--- May not be needed
}

void flashLED (int ledLeft, int ledRight, CRGB curColor, int msDelay) {
  leds[ledLeft] = curColor;   leds[ledRight] = curColor;
  FastLED.show();
  FastLED.delay(msDelay);
}    

void ledWave(CRGB maxColor, CRGB minColor, int msDelay, bool boolDirection) {
  // Debug Info:  
    Serial.print(F("Max Color: "));     Serial.print(maxColor);
    Serial.print(F("Min Color: "));     Serial.print(minColor);
    Serial.print(F("Delay: "));         Serial.print(msDelay);
    if (boolDirection) {
      Serial.print(F("Direction: Out"));
    } else {
      Serial.print(F("Direction: In"));
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
      Serial.print(F("cur. left: "));     Serial.print(ledLeft);
      Serial.print(F("cur. right: "));    Serial.print(ledRight);

    flashLED (ledLeft, ledRight, maxColor, msDelay);
    flashLED (ledLeft, ledRight, minColor, msDelay / 5);
  }

  // Flash the center LED if number is ODD and direction is IN
  if (!boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, msDelay / 5);
  }
}






// original loop from outside someone else
/*
for (int i = 0; i < NUM_LEDS; i++) {
  leds[i] = CRGB::Red;  //set the led to Pink
  FastLED.show();       //start the leds
  leds[i] = CRGB::Black;  //clear the led
  delay(50);          //Wait before moving to next let 
  }
  for (int i = (NUM_LEDS)-1; i >= 0; i--)
       {
  leds[i] = CRGB::Blue;  //set the led to Pink
  FastLED.show();       //start the leds
  leds[i] = CRGB::Black;  //clear the led
  delay(50);          //Wait before moving to previous let 
    }
} */

/*
void startup() {
for (int i = 0; i < NUM_LEDS; i++) {
  leds[i] = CRGB::Red;  //set the led to Pink
  FastLED.show();       //start the leds
  leds[i] = CRGB::Black;  //clear the led
  delay(50);          //Wait before moving to next let 
  }
for (int i = (NUM_LEDS)-1; i >= 0; i--)
       {
  leds[i] = CRGB::Blue;  //set the led to Pink
  FastLED.show();       //start the leds
  leds[i] = CRGB::Black;  //clear the led
  delay(50);          //Wait before moving to previous let 
    }
  fill_solid(leds, NUM_LEDS, CRGB::Green); 
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB::Black); 
  delay(1000);
}
*/