/*-----------------------------------------------------------------
    Program:        car_led_controller

    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, activity

    Date:           Added when finalized to production (2/6/2023)

    Author:         Corey Davis, Yves Avady

    Credits:        volt_measure by W.A Smith 
                    http://startingelectronics.org

                    Add credits for code obtained from elsewhere
                    

-----------------------------------------------------------------*/
#include <FastLED.h>

// Pins to device mapping
// D2 => In1 Relay (RelayPin1)
// D3 => In2 Relay (RelayPin2)
// D12 => LED Controller Signal (LED_Signal) 
// A2 => DRL Sense (DRL)
// A3 => Parking Lights Sense (PK_L)
// A4 => Horn Sense (HN)
// A5 => HiBeam Sense (HIBM)

// Pins
const int RelayPin1 = 2;
const int RelayPin2 = 3;
const int LED_Signal = 11;
const int DRL = A5;
const int PK_L = A4;
const int NH = A3;
const int HIBM = A2;

// Setup relay powerstates
// reverse if relay is wired differently
#define RELAY_ON      LOW
#define RELAY_OFF     HIGH

// number of analog samples to take per reading
#define NUM_SAMPLES     10

//LED Controller Section
#define LED_PIN         11
#define LED_TYPE        WS2811
#define NUM_LEDS        30
#define NUM_LEDS_HALF   NUM_LEDS / 2
#define NUM_LEDS_ODD    NUM_LEDS % 2

#define BRIGHTNESS      250
#define BRIGHTNESS_H    255
#define BRIGHTNESS_L    127
#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  128
#define MED_BRIGHTNESS  196

#define COLOR_ORDER     BRG
#define COLOR_TEMP      UncorrectedTemperature

CRGBArray<NUM_LEDS> leds;
//CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
#define VOLT_DIV_FACTOR 22.410  //voltage divider factor
// voltage multiplied by 22 when using voltage divider that
// divides by 22. 22.410 is the calibrated voltage divider
// value

int sum_HIBM = 0;
int sum_NH = 0;
int sum_PK_L = 0;
int sum_DRL = 0;
/*
unsigned char sample_count = 0;                    // sum of samples taken
unsigned char sample_count_HIBM = 0;
unsigned char sample_count_HN = 0;
unsigned char sample_count_PK_L = 0;
unsigned char sample_count_DRL = 0; // current sample number

//Yves not sure why we have float and unsigned char variables with the same name here. 
//I think removing the char declarations, will all you to remove the explicit declararions from the formulas below
*/

float sample_count = 0; 
float sample_count_HIBM = 0.0;
float sample_count_HN = 0.0;
float sample_count_PK_L = 0.0;
float sample_count_DRL = 0.0;            // calculated voltage



void setup()
{
    Serial.begin(9600);   // serial monitor for debugging
    delay( 125 );        // power-up safety delay

    // Set pins as an input or output pin
	  pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(LED_Signal, OUTPUT);
    pinMode(DRL, INPUT);
    pinMode(PK_L, INPUT);
    pinMode(NH, INPUT);
    pinMode(HIBM, INPUT);

    // Start LEDs
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(MAX_BRIGHTNESS);
    FastLED.setTemperature(COLOR_TEMP);
  
    // Initiate startup lighting sequence
    startupSequence();
}

void loop()
{
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum_DRL += analogRead(DRL);
        //sample_count_DRL++;
		    sum_PK_L += analogRead(PK_L);
        //sample_count_PK_L++;
		    sum_NH += analogRead(NH);
        //sample_count_HN++;
		    sum_HIBM += analogRead(HIBM);
        //sample_count_HIBM++;
        sample_count++;
        delay(100);
    
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.09V is the calibrated reference voltage
    sample_count_DRL = ((float)sum_DRL / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  sample_count_PK_L = ((float)sum_PK_L / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  sample_count_HN = ((float)sum_NH / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  sample_count_HIBM = ((float)sum_HIBM / (float)NUM_SAMPLES * 5.09) / 1024.0;

    // send voltage for display on Serial Monitor
	  Serial.print("Voltage of A2 = ");
	  Serial.print(sample_count_HIBM * VOLT_DIV_FACTOR);
    Serial.println (" V");
	  Serial.print("Voltage of A3 = ");
	  Serial.print(sample_count_HN * VOLT_DIV_FACTOR);
    Serial.println (" V");
	  Serial.print("Voltage of A4 = ");
	  Serial.print(sample_count_PK_L * VOLT_DIV_FACTOR);
    Serial.println (" V");
	  Serial.print("Voltage of A5 = ");
	  Serial.print(sample_count_DRL * VOLT_DIV_FACTOR);
    Serial.println (" V");
    sample_count = 0;
    sample_count_HIBM = 0;
	  sample_count_HN = 0;
	  sample_count_PK_L = 0;
	  sample_count_DRL = 0;
    sum_HIBM = 0;
	  sum_NH = 0;
	  sum_PK_L = 0;
	  sum_DRL = 0;

    if ( sample_count_DRL * 22.410 >= 1.00 &&  sample_count_DRL * 22.410 <= 10.00) {  //set lower voltage to 2v, set to 1v due to limitation of testing hardware
      //turn on relay1
      Serial.print("Turning on relay1 because A5 is ");
	    Serial.print(sample_count_DRL * 22.410);
      Serial.println (" V");      
      digitalWrite(RelayPin1, RELAY_ON);
      leds[NUM_LEDS] = CRGB(255,255,255); 
      FastLED.setBrightness( BRIGHTNESS_L ); //set half brightness
      FastLED.show();
      Serial.println("DRL Brightness level HALF");
      delay(500);
      }
	  if (sample_count_DRL * 22.410 >= 11.01){
        Serial.print("Brightening LED on relay1 because A5 is ");
	      Serial.print(sample_count_DRL * 22.410);
        Serial.println (" V");
        digitalWrite(RelayPin1, RELAY_ON);
        leds[NUM_LEDS] = CRGB(255,255,255); 
        FastLED.setBrightness( BRIGHTNESS_H ); //set full brightness
        FastLED.show();
        Serial.println("DRL Brightness level FULL");
        delay(500);
      } 
    if (sample_count_DRL * 22.410 == 0)
      {
          //trun off relay1
          Serial.print("Dimming LED off relay1 because A5 is ");
          digitalWrite(RelayPin1, RELAY_OFF);
          Serial.print(sample_count_DRL * 22.410);
          Serial.println (" V");   
	        delay(500);
    }
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

