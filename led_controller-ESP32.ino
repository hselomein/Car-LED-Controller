/*-----------------------------------------------------------------
    Program:        car_led_controller

    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		    activity

    Date:           Added when finalized to production (2/6/2023)

    Authors:         Corey Davis, Yves Avady, Jim Edmonds
-----------------------------------------------------------------*/
#include <FastLED.h>
#include <LiquidCrystal_I2C.h>

// Pins to device mapping
#define RELAY_PIN_1     14        // D14 => In1 Relay
#define RELAY_PIN_2     12        // D12 => In2 Relay
#define LED_PIN         13        // D13 => LED Controller Signal
#define DRL_PIN         34        // D34 => DRL Sense
#define PK_L_PIN        35        // D35 => Parking Lights Sense 
#define HORN_PIN        32        // D32 => Horn Sense
#define HIBM_PIN        33        // A33 => HiBeam Sense

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);  

//LED Controller Section
#define LED_TYPE        WS2811
#define NUM_LEDS        60
#define NUM_LEDS_HALF   NUM_LEDS / 2
#define NUM_LEDS_ODD    NUM_LEDS % 2
#define COLOR_ORDER     GRB
#define COLOR_TEMP      UncorrectedTemperature
CRGBArray<NUM_LEDS> leds;

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  95
#define MED_BRIGHTNESS  191

#define ANGRY_COLOR     CRGB(255,69,0)      //Orange
#define DEFAULT_COLOR   CRGB(255,255,255)   //White
#define LYFT_COLOR      CRGB(255,0,255)     //Magenta
#define UBER_COLOR      CRGB(255,165,0)     //Cyan

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define NUM_SAMPLES     20          // number of analog samples to take per reading
#define REF_VOLTAGE     5.09        // Reference Voltage
#define A2D_RESOLUTION  4096        // Reference Voltage
#define VOLT_DIV_FACTOR 22.368      //voltage divider factor
// voltage multiplied by 22 when using voltage divider that
// divides by 22. 22.368 is the calibrated voltage divider
#define VOLT_ADJ (REF_VOLTAGE * VOLT_DIV_FACTOR / A2D_RESOLUTION / NUM_SAMPLES);

#define VOLT_BUF        2
#define HI_VOLT         12
#define LO_VOLT         4

// Static test messages
#define LOADING         "Loading..."
#define PLEASE_WAIT     "Please Wait!!!"

// Startup Configuration (Constants)
#define msDELAY       20   //Number of ms LED stays on for.
#define numLOOPS      4   //Humber of passes over entire LED strip
#define brightCOLOR   CRGB( 255, 255, 255)
#define dimCOLOR      CRGB( 96, 96, 96)
#define offCOLOR      CRGB( 0, 0, 0)


void setup()
{
  // set up the LCD:
  lcd.init();     // Init LCD      
  lcd.backlight();      // Make sure backlight is on
  lcd.clear();      //clear lcd screen
  
  lcd.home(); //move cursor to 1st line on display
  lcd.print(LOADING);   
  lcd.setCursor(0,1); //move cursor to 2nd line on display
  lcd.print(PLEASE_WAIT);   

  //Serial.begin(115200);   // serial monitor for debugging
  FastLED.delay(250);           // power-up safety delay


  // Set pins as an input or output pin
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DRL_PIN, INPUT);
  pinMode(PK_L_PIN, INPUT);
  pinMode(HORN_PIN, INPUT);
  pinMode(HIBM_PIN, INPUT);

  // Start LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.setTemperature(COLOR_TEMP);

  // Initiate startup lighting sequence
  startupSequence();
  lcd.clear();  //clear lcd screen
  //lcd.autoscroll(); //enable scrolling on long lines
}

void loop()
{
  static char curMode = 1;
  static char curSample = 1;
  static float curDRL = 0;
  static float curPkL = 0;
  static float curHorn = 0;
  static float curHiBeam = 0;
  static bool RelayPin1State = false;

  curDRL += analogRead(DRL_PIN);
  curPkL += analogRead(PK_L_PIN);
  curHorn += analogRead(HORN_PIN);
  curHiBeam += analogRead(HIBM_PIN);
  curSample++;

  if (curSample > NUM_SAMPLES){
    curDRL *= VOLT_ADJ;
    curPkL *= VOLT_ADJ;
    curHorn *= VOLT_ADJ;
    curHiBeam *= VOLT_ADJ;
    curSample = 1;

    lcd.clear();    //clear lcd screen
    lcd.home();     //move cursor to 1st line on display
    //lcd.print(F("DRL: "));   lcd.print(curDRL);    lcd.print(F("V   "));
    //lcd.print(F("PkL: "));   lcd.print(curPkL);    lcd.print(F("V   "));
    //lcd.print(F("Hrn: "));   lcd.print(curHorn);   lcd.print(F("V   "));
    //lcd.print(F("HiBm: "));  lcd.print(curHiBeam); lcd.print(F("V   "));
     
    //Serial.print("Voltage of HIBM = ");   Serial.print(curHiBeam);    Serial.println ("V");
    //Serial.print("Voltage of Horn = ");   Serial.print(curHorn);      Serial.println ("V");
    //Serial.print("Voltage of PK_L = ");   Serial.print(curPkL);       Serial.println ("V");
    //Serial.print("Voltage of DRL = ");    Serial.print(curDRL);       Serial.println ("V");

    if (Abs(curDRL - LO_VOLT) < VOLT_BUF) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        //Serial.print("Turning on Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      FastLED.setBrightness(MIN_BRIGHTNESS); //set low brightness
      FastLED.show();
      //Serial.println("DRL Brightness level LOW");
    } else if (curDRL > (HI_VOLT - VOLT_BUF)) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        //Serial.print("Turning on Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      FastLED.setBrightness(MAX_BRIGHTNESS); //set max brightness
      FastLED.show();
      //Serial.println("DRL Brightness level MAX");
    } else if (curDRL < VOLT_BUF) {
      if (RelayPin1State) {
        RelayPin1State = false;
        //turn off relay1
        //Serial.print("Turning off Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_OFF);
      }
      FastLED.setBrightness(0); //turn off lights
      FastLED.show();
      //Serial.println("DRL Brightness level OFF");
    }

    if (curHorn > (HI_VOLT - VOLT_BUF)) {
      fill_solid(leds, NUM_LEDS, ANGRY_COLOR);  
      //lcd.setCursor(0,o); //move cursor to 2nd line on display
      //lcd.print(F("Color: Orange"));   
      //Serial.println("LED color set to Horn color (orange)");
    } else {
      fill_solid(leds, NUM_LEDS, DEFAULT_COLOR);  
      //lcd.setCursor(0,1); //move cursor to 2nd line on display
      //lcd.print(F("Color: White"));   
      //Serial.println("LED color set to Default color (white)");
    }

    curDRL = 0;
    curPkL = 0;
    curHorn = 0;
    curHiBeam = 0;

    lcd.print(F("DRL: "));   lcd.print(curDRL);    lcd.print(F("V   "));
    //lcd.print(F("PkL: "));   lcd.print(curPkL);    lcd.print(F("V   "));
    //lcd.print(F("Hrn: "));   lcd.print(curHorn);   lcd.print(F("V   "));
    //lcd.print(F("HiBm: "));  lcd.print(curHiBeam); lcd.print(F("V   "));
     
    //Serial.print("Voltage of HIBM = ");   Serial.print(curHiBeam);    Serial.println ("V");
    //Serial.print("Voltage of Horn = ");   Serial.print(curHorn);      Serial.println ("V");
    //Serial.print("Voltage of PK_L = ");   Serial.print(curPkL);       Serial.println ("V");
    //Serial.print("Voltage of DRL = ");    Serial.print(curDRL);       Serial.println ("V");

    FastLED.delay(1000); //Delay to prevent LCD flashing
  }
}

void startupSequence() {
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
  FastLED.delay(1000);                      //<--- May not be needed
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
