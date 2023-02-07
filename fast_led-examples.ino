/* Use this file to test led strip actions and bahaviors*/
#include <FastLED.h>
#define LED_PIN     11
#define NUM_LEDS    30
#define BRIGHTNESS  250
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define MIDDLE_LED  15

CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
    Serial.begin(9600);  
	  // Set RelayPin as an output pin
    int RelayPin = 2; 
	  pinMode(RelayPin, OUTPUT);
    
    //Start LEDs
		delay( 500 ); // power-up safety delay
    digitalWrite(RelayPin, LOW);
		FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
		FastLED.setBrightness(  BRIGHTNESS );
		currentPalette = RainbowColors_p;
		currentBlending = LINEARBLEND;

	}
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


// Yves contribution

void flashLED (int left, int right, int color, int delay) {
    leds[left] = color;
    leds[right] = color;
    FastLED.show();
    FastLED.delay(delay);
    leds[left] = CRGB::Black;              // Set to black for next loop iteration
    leds[right] = CRGB::Black; 
}    

void toCenter(int color, int delay, int numLED) {
    const int HALFWAY = numLED / 2;
    
    // get the center LED if number is odd
    if (numLED % 2) flashLED (HALFWAY, HALFWAY + 1, color, delay);

    for (int i = HALFWAY; i >= 0; i--) {
        Serial.print(F("left: "));
        Serial.print(i);
        Serial.print(F("right: "));
        Serial.print(HALFWAY - i);

        flashLED (i, HALFWAY - i, color, delay);
    }
}

void fromCenter(int color, int delay, int numLED) {
    const int HALFWAY = numLED / 2;

    for (int i = 0; i < HALFWAY; i++) {
        Serial.print(F("left: "));
        Serial.print(i);
        Serial.print(F("right: "));
        Serial.print(HALFWAY - i);

        flashLED (i, HALFWAY - i, color, delay);
    }

    // get the center LED if number is odd
    if (numLED % 2) flashLED (HALFWAY, HALFWAY + 1, color, delay);
}


const int DELAY = 25;
const int COLOR = CRGB::Blue;
const int NUM_LOOPS = 2;
void loop() {
  
for (int i = 0; i < NUM_LOOPS; i++){
    toCenter(COLOR, DELAY, NUM_LEDS);
    fromCenter(COLOR, DELAY, NUM_LEDS);
}

//Solid Color:
for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = COLOR;
}
FastLED.show();

}

// original loop from outside someone else
//void loop() {
//  startup();
//} 
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
