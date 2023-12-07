/*-----------------------------------------------------------------
    Program:        car_led_controller
    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		                activity. This is for the esp32 38pin device
    Date:           10/11/2023  
    Version:        .9b
    Authors:        Yves Avady
    Contriburtors:   Corey Davis, Jim Edmonds 
-----------------------------------------------------------------*/
//Build Configuration Options
  #define DEBUG false       //Enable serial output for debug, change to "false" to disable
  #define SCREENTEST false  //To enable the boot up screen test, change to, to disable change to "false"
  #define LED_MATRIX true   //Set to false if you want to use a 64x64 LED Matrix
//#define LED_STRIP         //for future development
  #define NUM_MODES 2       //How many modes will the mode button handle (2 for Uber and Lyft signs)
  #define LEFT_IND false    //enable left indicator code for testing
  #define RIGHT_IND false   //enable right indicator code for testing
  #define V9_PCB true       //enble if you are using V9 LED Controller PCB

//Arduino Standard
  //#include <stdio.h>
 //#include <Wire.h>
  #include <driver/adc.h>
  #include <esp_adc_cal.h>
  
// Pins to device mapping
  #define RELAY_PIN_1 18                // Pin 18 => In1 Relay
  #define LED_PIN     23                // Pin 23 => LED Controller Signal
  #define DRL_PIN     ADC1_CHANNEL_0    // Pin 39 => DRL Sense

  #if V9_PCB == false
  #define HORN_PIN    ADC1_CHANNEL_3  // Pin 39 => Horn Sense
  #define IND_L_PIN   ADC1_CHANNEL_6  // Pin 34 => Left Indicator 
  #define IND_R_PIN   ADC1_CHANNEL_7  // Pin 35 => Right Indicator Sense (Reserved)
  #endif
  #if V9_PCB 
  #include <SimpleButton.h>
  using namespace simplebutton;
  #define HORN_PIN    39  // Pin 39 => Horn Sense
  #define IND_L_PIN   34  // Pin 34 => Left Indicator 
  #define IND_R_PIN   35  // Pin 35 => Right Indicator Sense (Reserved)
  Button* Horn_Button = NULL;
  Button* Ind_L_Button = NULL;
  Button* Ind_R_Button = NULL;

  #endif
//Define lcd and led brightness
  #define MAX_BRIGHTNESS  255
  #define MIN_BRIGHTNESS  63
  #define MED_BRIGHTNESS  127

  #define RELAY_ON LOW
  #define RELAY_OFF HIGH

  #define NUM_SAMPLES     50              // number of analog samples to take per reading
  #define R1              47.0            // Resistor 1 value of voltage divider
  #define R2              10.0            // Resistor 2 value of voltage divider
  #define VOLT_DIV_FACTOR (R1+R2)/R2      //voltage divider factor
  static esp_adc_cal_characteristics_t ADC1_Characteristics;

  #define VOLT_BUF        1
  #define HI_VOLT         12
  #define LO_VOLT         2
  

// Startup Configuration (Constants)
  #define msDELAY  int(400 / NUM_PIXELS + 0.5)   //Number of ms LED stays on for.
  #define numLOOPS      4   //Humber of passes over entire LED strip

  static float curDRL   = 0.0f;
  static float curHorn  = 0.0f;
  static float curInd_L = 0.0f;
  static float curInd_R = 0.0f;

  

//EZ Button
  #include <ezButton.h> 
  ezButton modeButton(19);  // create ezButton object that attach to pin 34;
  #define DEBOUNCE_TIME 75 // the debounce time in millisecond, increase this time if it still chatters

//LED Strip
  #include <Adafruit_NeoPixel.h>

//LED Controller Section
  #define NUM_LEDS  134   //161 leds is the lenght of the hood weather strip, 36 for the COB strip
  #define NUM_LEDS_HALF   (NUM_LEDS - 1) / 2    //Subtract 1 to calculate indexes
  #define LEDS_PER_PIXEL 1
  #define NUM_PIXELS (NUM_LEDS / LEDS_PER_PIXEL)
  #define RGBW_STRIP false //for RGB Strips change to false
  #define RGBW_COLOR_ORDER NEO_GRBW //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  #define RGB_COLOR_ORDER NEO_RGB //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  
  #if RGBW_STRIP 
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, RGBW_COLOR_ORDER + NEO_KHZ800);
  //color definitions values, are expressed in rgbw format
  #define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
  #define DEFAULT_COLOR   leds.Color( 0,  0,  0,   255 )     //White
  #define LYFT_COLOR      leds.Color( 255,  0, 191,  0 )     //Magenta
  #define UBER_COLOR      leds.Color( 0,  255,  92,  0 )     //Seafoam Green
  #define BRIGHTCOLOR   leds.Color( 255, 255, 255, 255 )     //Full White
  #define DIMCOLOR      leds.Color(  50,  50,  50,  50 )     //Dim White
  #define OFFCOLOR      leds.Color(   0,   0,   0,   0 )     //Off
  #endif

  #if RGBW_STRIP == false
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, RGB_COLOR_ORDER + NEO_KHZ800);
  //color definitions values, are expressed in rgb format
  #define ANGRY_COLOR     leds.Color( 255, 60,  0   )     //Amber
  #define DEFAULT_COLOR   leds.Color( 200, 255, 255 )     //White
  #define LYFT_COLOR      leds.Color( 255, 0,   191 )     //Magenta
  #define UBER_COLOR      leds.Color( 0,  255,  92  )     //Seafoam Green
  #define BRIGHTCOLOR   leds.Color( 255,  255,  255 )     //Full White
  #define DIMCOLOR      leds.Color(  50,  50,   50  )     //Dim White
  #define OFFCOLOR      leds.Color(   0,   0,   0   )     //Off
  #endif

#if LED_MATRIX    
//LED Matrix Panel
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

//MatrixPanel_I2S_DMA dma_display;
  MatrixPanel_I2S_DMA *dma_display = nullptr;
//LED Martrix pin section
  #define PCB_TYPE 1  
                      /*-------------------------------------------------- 
                      Set PCB_TYPE to the appropaite PCB version
                      for LED Matrix Controller V8 use "1", 
                      for LED Matrix Controller V9 or Yves Version use "2",
                      for working pinout 2 use "3"
                      for waveguide standard pinout use "4",
                      ---------------------------------------------------*/

  #if PCB_TYPE==1
//This is configured using a P2 64x64 LED Matrix, which has an E pin.
//Pinout for LED Matrix Controller V8
  #define R1_PIN  25
  #define G1_PIN  26
  #define B1_PIN  33
  #define R2_PIN  14
  #define G2_PIN  12
  #define B2_PIN  13
  #define A_PIN   27
  #define B_PIN   2 
  #define C_PIN   5
  #define D_PIN   17
  #define E_PIN   32  
  #define LAT_PIN 4
  #define OE_PIN  15
  #define CLK_PIN 16
  #endif
 
// Pinout for LED Matrix Controller V9 or Yves Version
  #if PCB_TYPE==2
  #define R1_PIN  25
  #define G1_PIN  26
  #define B1_PIN  33
  #define R2_PIN  14
  #define G2_PIN  12
  #define B2_PIN  13
  #define A_PIN   27
  #define B_PIN   2 
  #define C_PIN   5
  #define D_PIN   17
  #define E_PIN   32  
  #define LAT_PIN 4
  #define OE_PIN  15
  #define CLK_PIN 16
  #endif
  
//working pinout 2  
  #if PCB_TYPE==3
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
  #endif

//wave share pinout
  #if PCB_TYPE==4
  #define R1_PIN  25
  #define G1_PIN  26
  #define B1_PIN  27
  #define R2_PIN  14
  #define G2_PIN  12
  #define B2_PIN  13
  #define A_PIN   23
  #define B_PIN   22 
  #define C_PIN   5
  #define D_PIN   17
  #define E_PIN   32  
  #define LAT_PIN 4
  #define OE_PIN  15
  #define CLK_PIN 16
  #endif
  
  //LED Matrix Initialization
  #define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
  #define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
  #define PANEL_CHAIN 1   // Total number of panels chained one to another

#include <logos.h>
#endif


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
            if (isLyftDisplayed == true) {isLyftDisplayed == false;}
            curColor = UBER_COLOR;
            txtColor = "UBER"; 
            if (DEBUG) Serial.println("LED color set to Uber Mode color (Seafoam Green)");         
            if (txtColor == "UBER" && isUberDisplayed == false){
#if LED_MATRIX            
              drawUberLogo(dma_display);
#endif              
              isUberDisplayed = true;
            } 
            break;
        case 2:
            if (isUberDisplayed == true) {isUberDisplayed == false;}
            curColor = LYFT_COLOR;
            txtColor = "LYFT";
            if (DEBUG) Serial.println("LED color set to Lyft Mode color (Magenta)");
            if (txtColor == "LYFT" && isLyftDisplayed == false){
#if LED_MATRIX            
            drawLyftLogo(dma_display);
#endif            
              isLyftDisplayed = true;
            }
            break;
        default:
            curColor = DEFAULT_COLOR;
            txtColor = "WHIT";
            if (DEBUG) Serial.println("LED color set to Default Mode color (White)");
#if LED_MATRIX            
            dma_display->fillScreen(dma_display->color565(0, 0, 0));
#endif            
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

#include <TaskLCD.h>

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
    if (msDelay) delay(msDelay);
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
  if (val > 0) return val;
  else return -val;
}

#if LED_MATRIX 
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
#endif
//-----------main program-----------------

void setup()
{
  // Start LEDs
  digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
  leds.begin();
  leds.show();
  leds.setBrightness(MAX_BRIGHTNESS);

  //delay(250); // power-up safety delay use for bad powersupplies, enable only if needed

  if (DEBUG) Serial.begin(115200);   // serial monitor for debugging
    
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
  pinMode(LED_PIN, OUTPUT);
  pinMode(DRL_PIN, INPUT);
  if (V9_PCB) pinMode(HORN_PIN, INPUT);
  if (V9_PCB) pinMode(IND_L_PIN, INPUT);
  if (V9_PCB) pinMode(IND_R_PIN, INPUT);
  if (LEFT_IND) pinMode(IND_L_PIN, INPUT);
  if (RIGHT_IND) pinMode(IND_R_PIN, INPUT);

  #if V9_PCB 
  Horn_Button = new Button(HORN_PIN, true); //this is for an inverted setup where HIGH is the idle state of the buttonsef
  Ind_L_Button = new Button(IND_L_PIN, true);
  Ind_R_Button = new Button(IND_R_PIN, true);
  #endif

  // Configure ADC
  if (DEBUG) {Serial.println("Start Init ADC");}
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  if (DEBUG) {Serial.println("Init ADC 1");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_11));
  //delay(150); // see if timing has effect on crashing
#if V9_PCB == false
  if (DEBUG) {Serial.println("Init ADC 2");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(HORN_PIN, ADC_ATTEN_DB_11));
  //delay(150); // see if timing has effect on crashing
#endif

#if V9_PCB == false
  if (RIGHT_IND) {
    if (DEBUG) {Serial.println("Init ADC 3");} 
    ESP_ERROR_CHECK(adc1_config_channel_atten(IND_R_PIN, ADC_ATTEN_DB_11));
    //delay(150); // see if timing has effect on crashing
  }
#endif

#if V9_PCB == false
  if (LEFT_IND) {
    if (DEBUG) {Serial.println("Init ADC 4");}
    ESP_ERROR_CHECK(adc1_config_channel_atten(IND_L_PIN, ADC_ATTEN_DB_11));
    //delay(150); // see if timing has effect on crashing
  }
#endif

#if LED_MATRIX 
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
    dma_display->fillScreen(dma_display->color565(255, 255, 255));
#endif


  // Initiate startup lighting sequence
    startupSequence(); 

#if LED_MATRIX 
  if (SCREENTEST) screentest();
#endif

  initTaskLCD();
}

void loop()
{
  static int    curSample = 1;
  static bool   RelayPin1State = false;

  curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics);
#if V9_PCB == false
  curHorn += esp_adc_cal_raw_to_voltage(adc1_get_raw(HORN_PIN), &ADC1_Characteristics);
  if (LEFT_IND) curInd_L += esp_adc_cal_raw_to_voltage(adc1_get_raw(IND_L_PIN), &ADC1_Characteristics);
  if (RIGHT_IND) curInd_R += esp_adc_cal_raw_to_voltage(adc1_get_raw(IND_R_PIN), &ADC1_Characteristics);
#endif
  curSample++;

  if (DEBUG) {
    Serial.print("Horn Voltage:"); Serial.println(curHorn);
    Serial.print("DRL Voltage:"); Serial.println(curDRL);
    Serial.print("Current Sample Number:"); Serial.println(curSample);
  }

  modeButton.loop();      // MUST call the loop() function first
  if (DEBUG) Serial.print("Mode Select Button State:");  Serial.println(modeButton.getState());
#if V9_PCB 
  Horn_Button->update();
  Ind_L_Button->update();
  Ind_R_Button->update();
#endif

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
#if V9_PCB == false
    
  if (LEFT_IND) curInd_L *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
  if (RIGHT_IND) curInd_R *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
#endif

    if (Abs(curDRL - LO_VOLT) < VOLT_BUF) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MIN_BRIGHTNESS);
#if LED_MATRIX      
      dma_display->setBrightness8(MIN_BRIGHTNESS);
#endif
      if (DEBUG) Serial.println("DRL Brightness level LOW");
    } else if (curDRL > (HI_VOLT - VOLT_BUF)) {
      if (!RelayPin1State) {
        RelayPin1State = true;
        //turn on relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      leds.setBrightness(MAX_BRIGHTNESS);
#if LED_MATRIX
      dma_display->setBrightness8(MAX_BRIGHTNESS);
#endif
      if (DEBUG) Serial.println("DRL Brightness level MAX");
    } else if (curDRL < VOLT_BUF) {
      leds.setBrightness(MAX_BRIGHTNESS);  //change back to 0 after solving left indicator / drl off issue
      if (RelayPin1State) {
        RelayPin1State = true; //change back to false after solving left indicator / drl off issue
        //turn off relay1
        digitalWrite(RELAY_PIN_1, RELAY_ON); //change back to RELAY_OFF after solving left indicator / drl off issue
#if LED_MATRIX
        dma_display->setBrightness8(MAX_BRIGHTNESS); //change back to 0 after solving left indicator / drl off issue
#endif
      }
      if (DEBUG) Serial.println("DRL Brightness level OFF");
    }

#if V9_PCB == false
    if (curHorn > VOLT_BUF) leds.fill(ANGRY_COLOR); 
#endif
#if V9_PCB 
  int ledLeft = 0; 
  int ledRight = 0;
  int offset;
  bool currentHornButtonState = Horn_Button->getState();
  bool currentInd_LButtonState = Ind_L_Button->getState();
  bool currentInd_RButtonState = Ind_R_Button->getState();

    if (!currentHornButtonState) leds.fill(ANGRY_COLOR);
    else if (!currentInd_RButtonState && currentInd_LButtonState) {
      for (int i = 1; i <= NUM_LEDS_HALF; i++){
        ledLeft = NUM_LEDS_HALF - i;
        leds.setPixelColor(ledLeft, ANGRY_COLOR);
        delay(int(400 / NUM_PIXELS + 0.5));
        leds.show();
      }
          leds.show();
          } 
    else if (!currentInd_LButtonState && currentInd_RButtonState) {
      for (int o = NUM_LEDS_HALF; o <= NUM_LEDS; o++){
        leds.setPixelColor(o, ANGRY_COLOR);
        delay(int(400 / NUM_PIXELS + 0.5));
        leds.show();
      }
          leds.show();
          } 
        else if (!currentInd_RButtonState && !currentInd_LButtonState) {
      for (int p = 1; p <= NUM_LEDS_HALF; p++) {
    // Set current left and right LEDs based on the direction
        ledLeft = NUM_LEDS_HALF - p;
        offset = 1; 
        ledRight = NUM_LEDS - ledLeft -1;
        leds.setPixelColor(ledLeft, ANGRY_COLOR);              leds.setPixelColor(ledRight, ANGRY_COLOR);
        delay(int(400 / NUM_PIXELS + 0.5));
        leds.show();
      }
          leds.show();
          } 
    else leds.fill(curMode.curColor);
#endif 
    leds.show();           
    curSample = 1;
    curDRL = 0;
    curHorn = 0;
    if (LEFT_IND) curInd_L = 0;
    if (RIGHT_IND) curInd_R = 0;
  }
}
