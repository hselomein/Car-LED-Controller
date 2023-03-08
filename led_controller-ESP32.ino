/*-----------------------------------------------------------------
    Program:        car_led_controller

    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		    activity. This is for the esp32

    Date:           Added when finalized to production (2/6/2023)

    Authors:         Corey Davis, Yves Avady, Jim Edmonds
-----------------------------------------------------------------*/
#include <stdio.h>

#include <Adafruit_NeoPixel.h>

#include <ezButton.h> 

#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#include <driver/adc.h>
#include <esp_adc_cal.h>

// Pins to device mapping
#define RELAY_PIN_1     14        // D14 => In1 Relay
#define RELAY_PIN_2     27        // D12 => In2 Relay
#define LED_PIN         13        // D13 => LED Controller Signal
#define DRL_PIN         ADC1_CHANNEL_5        // Pin 33 => DRL Sense
#define HORN_PIN        ADC1_CHANNEL_4        // Pin 32 => Horn Sense
ezButton modeButton(23);  // create ezButton object that attach to pin 23;
//#define PK_L_PIN        35        // D35 => Parking Lights Sense 
//#define HIBM_PIN        33        // A33 => HiBeam Sense

// set the LCD address to 0x27 for a 16 chars and 2 line display
#define LCD_COLS  16
#define LCD_ROWS  2 
#define LCD_UPDATE_INTERVAL 600   // How fast to update LCD in ms
hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip

//LED Controller Section
#define NUM_LEDS        193  //193 leds is the lenght of the hood weather strip + 15 leds on each side to show thru the headlights.
#define NUM_LEDS_HALF   (NUM_LEDS - 1) / 2    // Subtract 1 to calculate indexes
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  95
#define MED_BRIGHTNESS  127

#define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
#define DEFAULT_COLOR   leds.Color( 0,  0,  0,   255 )     //White
#define LYFT_COLOR      leds.Color( 255,  0, 255,  0 )     //Magenta
#define UBER_COLOR      leds.Color( 0,  165, 255,  0 )     //Cyan
#define brightCOLOR   leds.Color( 255, 255, 255, 255 ) 
#define dimCOLOR      leds.Color(  50,  50,  50,  50 ) 
#define offCOLOR      leds.Color(   0,   0,   0,   0 )

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define NUM_SAMPLES     10              // number of analog samples to take per reading
#define R1              47.0            // Resistor 1 value of voltage divider
#define R2              10.0            // Resistor 2 value of voltage divider
#define VOLT_DIV_FACTOR (R1+R2)/R2      //voltage divider factor
static esp_adc_cal_characteristics_t ADC1_Characteristics;

#define VOLT_BUF        1
#define HI_VOLT         12
#define LO_VOLT         2

// Startup Configuration (Constants)
#define msDELAY       50   //Number of ms LED stays on for.
#define numLOOPS      4   //Humber of passes over entire LED strip

#define DEBOUNCE_TIME  100 // the debounce time in millisecond, increase this time if it still chatters

static float curDRL    = 0.0f;
static float curHorn   = 0.0f;
//static double curPkL    = 0.0;
//static double curHiBeam = 0.0;

#define NUM_MODES 2

class cModes {
  public:
    int curColor;
    String txtColor;

    void Increment () {
      if (modeButton.getCount() > NUM_MODES) { modeButton.resetCount(); }
      switch (modeButton.getCount()) {
        case 1:
            curColor = UBER_COLOR;
            txtColor = "CYAN";          
            Serial.println("LED color set to Uber Mode color (Cyan)");
            break;
        case 2:
            curColor = LYFT_COLOR;
            txtColor = "MGTA";
            Serial.println("LED color set to Lyft Mode color (Magenta)");
            break;
        default:
            curColor = DEFAULT_COLOR;
            txtColor = "WHIT";
            Serial.println("LED color set to Default Mode color (White)");
            break;
      }
    }

    void Init() {
      curColor = DEFAULT_COLOR;
      txtColor = "WHIT";
      delay(400);
      modeButton.loop();      // MUST call the loop() function first
      modeButton.getCount();
      modeButton.resetCount();
    }
};
cModes curMode;

bool FirstLoop = true;

void setup()
{
  Serial.begin(115200);   // serial monitor for debugging
  delay(250); // power-up safety delay
    
  //setup the button
  modeButton.setDebounceTime(DEBOUNCE_TIME);  // set debounce time to 50 milliseconds    
  modeButton.setCountMode(COUNT_RISING);      // set a rising count for button

  // set up the LCD:
  lcd.begin(LCD_COLS, LCD_ROWS); //begin() will automatically turn on the backlight
  lcd.clear();            //clear the display  
  lcd.home();             //move cursor to 1st line on display
  lcd.print("LOADING");   
  lcd.setCursor(0,1);     //move cursor to 2nd line on display
  lcd.print("PLEASE WAIT");   

  // Set pins as an input or output pin
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  pinMode(DRL_PIN, INPUT);
  pinMode(HORN_PIN, INPUT);
  //pinMode(PK_L_PIN, INPUT);
  //pinMode(HIBM_PIN, INPUT);

  // Configure ADC
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_11));
  ESP_ERROR_CHECK(adc1_config_channel_atten(HORN_PIN, ADC_ATTEN_DB_11));

  // Start LEDs
  digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
  leds.begin();
  leds.show();
  leds.setBrightness(MAX_BRIGHTNESS);

  // Initiate startup lighting sequence
  startupSequence();

  // Create task on Core 1 to Update LCD
  xTaskCreatePinnedToCore(
        taskLCDUpdates,   /* Function to implement the task */
        "taskLCDUpdates", /* Name of the task */
        10000,            /* Stack size in words */
        NULL,             /* Task input parameter */
        0,                /* Priority of the task */
        NULL,             /* Task handle. */
        1);               /* Core where the task should run */
}

void taskLCDUpdates( void * pvParameters ){
  char tmpMessage[16];
  
  lcd.clear();    //clear the display and home the cursor
 
  sprintf(tmpMessage, "Color  DRL  Horn"); 
  lcd.setCursor(0,0); //move cursor to 1st line on display
  lcd.print(tmpMessage);
  delay(50); 

  while(true){
    if (curHorn > VOLT_BUF) {
      sprintf(tmpMessage, "ORNG %04.1fV %04.1fV", curDRL, curHorn);
    } else {
      sprintf(tmpMessage, "%s %04.1fV %04.1fV", curMode.txtColor, curDRL, curHorn);  
    }
    lcd.setCursor(0,1); //move cursor to 2nd line on display
    lcd.print(tmpMessage);

    delay(LCD_UPDATE_INTERVAL);
  }
}

void loop()
{
  static int    curSample = 1;
  static bool   RelayPin1State = false;

  curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics);
  curHorn += esp_adc_cal_raw_to_voltage(adc1_get_raw(HORN_PIN), &ADC1_Characteristics);
  //curPkL += analogRead(PK_L_PIN);
  //curHiBeam += analogRead(HIBM_PIN);
  curSample++;

  modeButton.loop();      // MUST call the loop() function first
  Serial.println(modeButton.getState());
  if (FirstLoop) {
    curMode.Init();
    FirstLoop = false;
  } else {
    curMode.Increment();
  }

  if (curSample > NUM_SAMPLES){
    // Adjust voltages
    curDRL *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
    curHorn *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
    //curPkL *= VOLT_ADJ;
    //curHiBeam *= VOLT_ADJ;

    if (Abs(curDRL - LO_VOLT) < VOLT_BUF) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MIN_BRIGHTNESS);
      Serial.println("DRL Brightness level LOW");
    } else if (curDRL > (HI_VOLT - VOLT_BUF)) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MAX_BRIGHTNESS);
      Serial.println("DRL Brightness level MAX");
    } else if (curDRL < VOLT_BUF) {
      leds.setBrightness(0);
      if (RelayPin1State) {
        RelayPin1State = false;
        //turn off relay1
        digitalWrite(RELAY_PIN_1, RELAY_OFF);
      }
      Serial.println("DRL Brightness level OFF");
    }

    if (curHorn > VOLT_BUF) {
      leds.fill(ANGRY_COLOR);  
    } else {
      leds.fill(curMode.curColor);  
    }

    leds.show();                      //<--- May not be needed
    
    curSample = 1;
    curDRL = 0;
    curHorn = 0;
    //curPkL = 0;
    //curHiBeam = 0;
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

  // Turn DefaultSolid Color:
  leds.fill(DEFAULT_COLOR);
}

void ledWave(uint32_t maxColor, uint32_t minColor, int msDelay, bool boolDirection) {
  int ledLeft = 0; int ledRight = 0;
  int offset;
  for (int i = 1; i <= NUM_LEDS_HALF; i++) {
    // Set current left and right LEDs based on the direction
    if (boolDirection) {                //Out 
      ledLeft = NUM_LEDS_HALF - i;
      offset = 1; 
    } else {                              //In
      ledLeft = i; 
      offset = -1;
    }
    ledRight = NUM_LEDS - ledLeft -1;

    leds.setPixelColor(ledLeft, maxColor);              leds.setPixelColor(ledRight, maxColor);
    leds.setPixelColor(ledLeft + offset, minColor);     leds.setPixelColor(ledRight - offset, minColor);
    if (msDelay) {
      delay(msDelay);
    }
    leds.show();
  }
}

float Abs(float val) {
  if (val > 0) { return val; }
  else { return -val; }
}
