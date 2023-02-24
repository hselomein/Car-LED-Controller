/*-----------------------------------------------------------------
    Program:        car_led_controller

    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		    activity

    Date:           Added when finalized to production (2/6/2023)

    Authors:         Corey Davis, Yves Avady, Jim Edmonds
-----------------------------------------------------------------*/
#include <stdio.h>
#include <Adafruit_NeoPixel.h>

//#include <LiquidCrystal_I2C.h> //remove for new library
//#include <Wire.h>
//#include <hd44780.h>                       // main hd44780 header
//#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header


// Pins to device mapping
#define RELAY_PIN_1     14        // D14 => In1 Relay
#define RELAY_PIN_2     27        // D12 => In2 Relay
#define LED_PIN         13        // D13 => LED Controller Signal
#define DRL_PIN         34        // D34 => DRL Sense
#define PK_L_PIN        35        // D35 => Parking Lights Sense 
#define HORN_PIN        32        // D32 => Horn Sense
#define HIBM_PIN        33        // A33 => HiBeam Sense

// set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27,16,2); //remove for new library
//#define LCD_COLS  16
//#define LCD_ROWS  2 
//hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

//LED Controller Section
#define NUM_LEDS        288
#define NUM_LEDS_HALF   NUM_LEDS / 2
#define NUM_LEDS_ODD    NUM_LEDS % 2
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  95
#define MED_BRIGHTNESS  127

#define ANGRY_COLOR     leds.Color( 255, 191,  0,   0 )     //Amber
#define DEFAULT_COLOR   leds.Color( 0,    0,  0, 255 )     //White
#define LYFT_COLOR      leds.Color( 255,  0, 255,  0 )     //Magenta
#define UBER_COLOR      leds.Color( 0,  165, 255,  0 )     //Cyan

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define NUM_SAMPLES     1000          // number of analog samples to take per reading
#define A2D_RESOLUTION  4096          // Resolution of the A2D converter (2 ^ number of bits)
#define REF_VOLTAGE     5.17          // Reference Voltage
#define R1              47            // Resistor 1 value of voltage divider
#define R2              10            // Resistor 2 value of voltage divider
#define VOLT_DIV_FACTOR (R1+R2)/R2    //voltage divider factor
// voltage multiplied by 22 when using voltage divider that
// divides by 22. 22.368 is the calibrated voltage divider
#define VOLT_ADJ (REF_VOLTAGE * VOLT_DIV_FACTOR / A2D_RESOLUTION / NUM_SAMPLES);

#define VOLT_BUF        2
#define HI_VOLT         12
#define LO_VOLT         3

// Startup Configuration (Constants)
#define msDELAY       1   //Number of ms LED stays on for.
#define numLOOPS      4   //Humber of passes over entire LED strip
#define brightCOLOR   leds.Color( 255, 255, 255, 255 ) 
#define dimCOLOR      leds.Color(  50,  50,  50,  50 ) 
#define offCOLOR      leds.Color(   0,   0,   0,   0 )


void setup()
{
  // set up the LCD:
  //lcd.begin(LCD_COLS, LCD_ROWS); //begin() will automatically turn on the backlight
  //lcd.init();     // Init LCD      
  //lcd.backlight();      // Make sure backlight is on
  //lcd.clear();      //clear the display and home the cursor
  
  //lcd.home(); //move cursor to 1st line on display
  //lcd.print(LOADING);   
  //lcd.setCursor(0,1); //move cursor to 2nd line on display
  //lcd.print(PLEASE_WAIT);   

  Serial.begin(115200);   // serial monitor for debugging
  delay(250);             // power-up safety delay


  // Set pins as an input or output pin
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DRL_PIN, INPUT);
  pinMode(PK_L_PIN, INPUT);
  pinMode(HORN_PIN, INPUT);
  pinMode(HIBM_PIN, INPUT);

  // Start LEDs
  digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
  leds.begin();
  leds.show();
  leds.setBrightness(MAX_BRIGHTNESS);

  // Initiate startup lighting sequence
  startupSequence();

  //lcd.clear();  //clear lcd screen
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
  char tmpMessage[16];


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

    //lcd.clear();    //clear the display and home the cursor
    //lcd.home();     //move cursor to 1st line on display

    if (Abs(curDRL - LO_VOLT) < VOLT_BUF) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        //Serial.print("Turning on Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MIN_BRIGHTNESS);
      Serial.println("DRL Brightness level LOW");
    } else if (curDRL > (HI_VOLT - VOLT_BUF)) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        //Serial.print("Turning on Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MAX_BRIGHTNESS);
      Serial.println("DRL Brightness level MAX");
    } else if (curDRL < VOLT_BUF) {
      leds.setBrightness(0);
      if (RelayPin1State) {
        RelayPin1State = false;
        //turn off relay1
        //Serial.print("Turning off Relay 1 because DRL is: ");  Serial.print(curDRL);   Serial.println ("V");      
        digitalWrite(RELAY_PIN_1, RELAY_OFF);
      }
      Serial.println("DRL Brightness level OFF");
    }

    if (curHorn > VOLT_BUF) {
      leds.fill(ANGRY_COLOR);  
      //lcd.setCursor(0,0); //move cursor to 2nd line on display
      //lcd.print("Color: Orange");
      //delay(3);
      //Serial.println("LED color set to Horn color (orange)");
    } else {
      leds.fill(DEFAULT_COLOR);  
      //lcd.setCursor(0,1); //move cursor to 2nd line on display
      //lcd.print("Color: White");
      //delay(3);   
      //Serial.println("LED color set to Default color (white)");
    }

    leds.show();                      //<--- May not be needed
    sprintf(tmpMessage, "DRL: %.2fV", curDRL);  
     
    Serial.println(tmpMessage);
    //Serial.print("Voltage of Horn = ");   Serial.print(curHorn);      Serial.println ("V");
    //Serial.print("Voltage of PK_L = ");   Serial.print(curPkL);       Serial.println ("V");
    //Serial.print("Voltage of DRL = ");    Serial.print(curDRL);       Serial.println ("V");


    //lcd.print(tmpMessage);
    //lcd.print(F("DRL: "));   lcd.print(curDRL);    lcd.print(F("V   "));
    //lcd.print(F("PkL: "));   lcd.print(curPkL);    lcd.print(F("V   "));
    //lcd.print(F("Hrn: "));   lcd.print(curHorn);   lcd.print(F("V   "));
    //lcd.print(F("HiBm: "));  lcd.print(curHiBeam); lcd.print(F("V   "));

    delay(100); //Delay to prevent LCD flashing

    curDRL = 0;
    curPkL = 0;
    curHorn = 0;
    curHiBeam = 0;
  }
}

void startupSequence() {
  // Loop 4 times
  //  1 - Towards center clear trail
  //  2 - Away from center clear trail
  //  3 - Towards center remain partial brightness
  //  4 - Away from center remain full brightness 
  uint32_t curDimColor = offCOLOR;
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

  // Turn Solid Color:              //<--- May not be needed
  leds.fill(brightCOLOR);           //<--- May not be needed
  delay(msDELAY);                   //<--- May not be needed
}

void flashLED (int ledLeft, int ledRight, uint32_t curColor, int msDelay) {
  leds.setPixelColor(ledLeft, curColor);   leds.setPixelColor(ledRight, curColor);
  leds.show();
  delay(msDelay);
}    

void ledWave(uint32_t maxColor, uint32_t minColor, int msDelay, bool boolDirection) {
  // Flash the center LED if number is ODD and direction is OUT
  if (boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, 0);
  }

  int ledLeft = 0; int ledRight = 0;
  for (int i = 0; i < NUM_LEDS_HALF; i++) {
    // Set current left and right LEDs based on the direction
    if (boolDirection) { ledLeft = NUM_LEDS_HALF - i; }  //Out
    else { ledLeft = i; }                                //In
    ledRight = NUM_LEDS - ledLeft;

    flashLED (ledLeft, ledRight, maxColor, msDelay);
    flashLED (ledLeft, ledRight, minColor, 0);
  }

  // Flash the center LED if number is ODD and direction is IN
  if (!boolDirection && NUM_LEDS_ODD) {
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, maxColor, msDelay);
    flashLED (NUM_LEDS_HALF, NUM_LEDS_HALF + 1, minColor, 0);
  }
}

float Abs(float val) {
  if (val > 0) { return val; }
  else { return -val; }
}
