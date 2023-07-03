/*-----------------------------------------------------------------
    Program:        car_led_controller
    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		                activity. This is for the esp32 38pin device
    Date:           5/18/2023
    Version:        1
    Authors:        Yves Avady
    Contriburtors:   Corey Davis, Jim Edmonds 
-----------------------------------------------------------------*/
//Build Configuration Options
#define DEBUG true      //Enable serial output for debug, change to "false" to disable
#define SCREENTEST true //To enable the boot up screen test, change to, to disable change to "false"
//#define NO_LED_MATRIX   //for future development
//#define NO_LED_STRIP    //for future development
#define NUM_MODES 2

#include <logos.h>
#include <TaskLCD.h>

//Arduino Standard
  #include <stdio.h>
  #include <Wire.h>
  #include <driver/adc.h>
  #include <esp_adc_cal.h>

  // Pins to device mapping
  #define RELAY_PIN_1 18        // D18 => In1 Relay
  #define RELAY_PIN_2 19        // D19 => In2 Relay
  #define LED_PIN     23        // D23 => LED Controller Signal
  #define DRL_PIN     ADC1_CHANNEL_0        // Pin 33 => DRL Sense
  #define HORN_PIN    ADC1_CHANNEL_3       // Pin 32 => Horn Sense
  //#define PK_L_PIN        35        // D35 => Parking Lights Sense (Reserved) 
  //#define HIBM_PIN        33        // D33 => HiBeam Sense (Reserved)

  //Define lcd and led brightness
  #define MAX_BRIGHTNESS  255
  #define MIN_BRIGHTNESS  63
  #define MED_BRIGHTNESS  127

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
  #define msDELAY       0   //Number of ms LED stays on for.
  #define numLOOPS      4   //Humber of passes over entire LED strip

//EZ Button
  #include <ezButton.h> 
  ezButton modeButton(34);  // create ezButton object that attach to pin 34;
  #define DEBOUNCE_TIME 75 // the debounce time in millisecond, increase this time if it still chatters

//LED Strip
  #include <Adafruit_NeoPixel.h>

  //LED Controller Section
  #define NUM_LEDS        193  //193 leds is the lenght of the hood weather strip including 15 leds on each side to show thru the headlights.
  #define NUM_LEDS_HALF   (NUM_LEDS - 1) / 2    // Subtract 1 to calculate indexes
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

  //color definitions values, are expressed in rgbw format
  #define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
  #define DEFAULT_COLOR   leds.Color( 0,  0,  0,   255 )     //White
  #define LYFT_COLOR      leds.Color( 255,  0, 191,  0 )     //Magenta
  #define UBER_COLOR      leds.Color( 0,  255,  92,  0 )     //Seafoam Green
  #define BRIGHTCOLOR   leds.Color( 255, 255, 255, 255 )     //Full White
  #define DIMCOLOR      leds.Color(  50,  50,  50,  50 )     //Dim White
  #define OFFCOLOR      leds.Color(   0,   0,   0,   0 )     //Off

//LED Matrix Panel
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

  //MatrixPanel_I2S_DMA dma_display;
  MatrixPanel_I2S_DMA *dma_display = nullptr;

  //LED Martrix pin section
  //This is configured using a P2 64x64 LED Matrix, which has an E pin.
  #define R1_PIN  32
  #define G1_PIN  33
  #define B1_PIN  25
  #define R2_PIN  26
  #define G2_PIN  27
  #define B2_PIN  14
  #define A_PIN   13
  #define B_PIN   15 
  #define C_PIN   2
  #define D_PIN   4
  #define E_PIN   12  
  #define LAT_PIN 17
  #define OE_PIN  5
  #define CLK_PIN 16

  //LED Matrix Initialization
  #define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
  #define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
  #define PANEL_CHAIN 1   // Total number of panels chained one to another

static float curDRL    = 0.0f;
static float curHorn   = 0.0f;
//static double curPkL    = 0.0;
//static double curHiBeam = 0.0;

//-------------functions-----------------
class cModes {
  public:
    int curColor;
    String txtColor;
    bool isUberDisplayed;
    bool isLyftDisplayed;

    void Increment () {
      if (modeButton.getCount() > NUM_MODES) { modeButton.resetCount(); }
      switch (modeButton.getCount()) {
        case 1:
            if (isLyftDisplayed == true) {isLyftDisplayed = false;}
            curColor = UBER_COLOR;
            txtColor = "UBER"; 
            if (DEBUG) {Serial.println("LED color set to Uber Mode color (Seafoam Green)");}         
            if (txtColor == "UBER" && isUberDisplayed == false){
              drawUberLogo();
              isUberDisplayed = true;
            } 
            break;
        case 2:
            if (isUberDisplayed == true) {isUberDisplayed = false;}
            curColor = LYFT_COLOR;
            txtColor = "LYFT";
            if (DEBUG) {Serial.println("LED color set to Lyft Mode color (Magenta)");}
            if (txtColor == "LYFT" && isLyftDisplayed == false){
              drawLyftLogo();
              isLyftDisplayed = true;
            }
            break;
        default:
            curColor = DEFAULT_COLOR;
            txtColor = "WHIT";
            if (DEBUG) {Serial.println("LED color set to Default Mode color (White)");}
            dma_display->fillScreen(color565(0, 0, 0));
            isLyftDisplayed = false;
            isUberDisplayed = false;
            break;
      }
    }

    void Init() {
      curColor = DEFAULT_COLOR;
      txtColor = "WHIT";
      delay(50);
      modeButton.loop();      // MUST call the loop() function first
      modeButton.getCount();
      modeButton.resetCount();
    }
};
cModes curMode;

bool firstLoop = true;

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

void startupSequence() {
  // Loop 4 times
  //  1 - Towards center clear trail
  //  2 - Away from center clear trail
  //  3 - Towards center remain partial brightness
  //  4 - Away from center remain full brightness 
  uint32_t curDimColor = OFFCOLOR;
  for (int i = 0; i < numLOOPS; i++){
    switch(i) {
      case 2:
        curDimColor = DIMCOLOR;
        break;
      case 3:
        curDimColor = BRIGHTCOLOR;
        break;
      default:
        curDimColor = OFFCOLOR;
    }
    // Perform the LED wave effect
    ledWave(BRIGHTCOLOR, curDimColor, msDELAY, i % 2);
  }

  // Turn DefaultSolid Color:
  leds.fill(DEFAULT_COLOR);
  leds.show();
}

float Abs(float val) {
  if (val > 0) { return val; }
  else { return -val; }
}

void screentest() {
  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
  delay(100);
  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(100);
  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width() - 1, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width() - 1, 0, 0, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  delay(100);
  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  delay(100);
  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
  delay(100);
  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));
}

//-----------main program-----------------

void setup()
{
  if (DEBUG) {Serial.begin(115200);}   // serial monitor for debugging
  
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

  // LED MATRIX Module configuration
    HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
    HUB75_I2S_CFG mxconfig(
      PANEL_RES_X,  // module width
      PANEL_RES_Y,  // module height
      PANEL_CHAIN,   // Chain length
      _pins // pin mapping
    );

    //mxconfig.gpio.e = 12;
    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::FM6124;

    // Display Setup
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(MAX_BRIGHTNESS); //5-255 led matrix does not display using values lower than 5
    dma_display->clearScreen();
    dma_display->fillScreen(color565(255, 255, 255));

  // Start LEDs
    digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
    leds.begin();
    leds.show();
    leds.setBrightness(MAX_BRIGHTNESS);

  // Initiate startup lighting sequence
    startupSequence(); 

  if (SCREENTEST) {screentest();}

  initTaskLCD();
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

  if (DEBUG) {
    Serial.print("Horn Voltage:"); Serial.println(curHorn);
    Serial.print("DRL Voltage:"); Serial.println(curDRL);
    Serial.print("Current Sample Number:"); Serial.println(curSample);
  }

  modeButton.loop();      // MUST call the loop() function first
  if (DEBUG) {Serial.print("Mode Select Button State:");  Serial.println(modeButton.getState());}

  if (firstLoop) {
    curMode.Init();
    firstLoop = false;
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
      dma_display->setBrightness8(MIN_BRIGHTNESS);
      if (DEBUG) {Serial.println("DRL Brightness level LOW");}
    } else if (curDRL > (HI_VOLT - VOLT_BUF)) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MAX_BRIGHTNESS);
      dma_display->setBrightness8(MAX_BRIGHTNESS);
      if (DEBUG) {Serial.println("DRL Brightness level MAX");}
    } else if (curDRL < VOLT_BUF) {
      leds.setBrightness(0);
      if (RelayPin1State) {
        RelayPin1State = false;
        //turn off relay1
        digitalWrite(RELAY_PIN_1, RELAY_OFF);
        dma_display->setBrightness8(0);
      }
      if (DEBUG) {Serial.println("DRL Brightness level OFF");}
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
