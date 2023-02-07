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



void loop() {
  int y;
  int x;
  x = NUM_LEDS / 2;
  y = (NUM_LEDS / 2) + 1;
/*
  for (int i = 0; x <= NUM_LEDS; i++) {

   // Serial.print(F("i= "));
   // Serial.print(i);
   // Serial.print(F("    x= "));
   // Serial.print(x++);
   // Serial.print(F("    y= "));
   // Serial.println(y--);
    x++;
    y--;
    leds[x] = CRGB::Blue;
    leds[y] = CRGB::Blue;
    FastLED.show();
    FastLED.delay(25);
    leds[x] = CRGB::Black;              // Set to black for next loop iteration
    leds[y] = CRGB::Black;
  }
  */
for (int i = 30 -1; i >= 0; i--) {

    //Serial.print(F("i= "));
    //Serial.print(i);
    //Serial.print(F("    x= "));
    //Serial.print(x++);
    //Serial.print(F("    y= "));
    //Serial.println(y--);
    x--;
    y++;
    leds[x] = CRGB::Blue;
    leds[y] = CRGB::Blue;
    FastLED.show();
    FastLED.delay(25);
    leds[x] = CRGB::Black;              // Set to black for next loop iteration
    leds[y] = CRGB::Black;
}
  }
/*
  for (int i = (NUM_LEDS)-1; y >= 0; i--) {
    Serial.print(F("i= "));
    Serial.print(i);
    Serial.print(F("    x= "));
    Serial.print(x++);
    Serial.print(F("    y= "));
    Serial.println(y--);

    leds[x] = CRGB::Blue;
    leds[y] = CRGB::Blue;
    FastLED.show();
    delay(25);
    leds[x] = CRGB::Black;              // Set to black for next loop iteration
    leds[y] = CRGB::Black;    
  }
}
*/

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
