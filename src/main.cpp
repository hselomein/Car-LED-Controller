/*-----------------------------------------------------------------
    Program:        car_led_controller
    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		                activity. This is for the esp32 38pin device
    Date:           2/27/2024  
    Version:        .95b
    Authors:        Yves Avady
    Contriburtors:   Corey Davis, Jim Edmonds 
-----------------------------------------------------------------*/
//Build Configuration Options
  #define DEBUG false       //Enable serial output for debug, change to "false" to disable
  #define SCREENTEST false  //To enable the boot up screen test, change to, to disable change to "false"
  #define LED_MATRIX true   //Set to false if you want to use a 64x64 LED Matrix
//#define LED_STRIP         //for future development (turns off the led strip)
  #define NUM_MODES 2       //How many modes will the mode button handle (2 for Uber and Lyft signs)
  #define LEFT_IND false    //enable left indicator code for testing
  #define RIGHT_IND false   //enable right indicator code for testing
  #define PCB_V9 true       //enble if you are using V9 LED Controller PCB

//Arduino Standard
  #include <driver/adc.h>
  #include <esp_adc_cal.h>
  
// Pins to device mapping
  #define RELAY_PIN_1 18                // Pin 18 => In1 Relay
  #define LED_PIN     23                // Pin 23 => LED Controller Signal
  #define DRL_PIN     ADC1_CHANNEL_0    // Pin 39 => DRL Sense

  #if PCB_V9 == false
  #define HORN_PIN    ADC1_CHANNEL_3  // Pin 39 => Horn Sense
  #define IND_L_PIN   ADC1_CHANNEL_6  // Pin 34 => Left Indicator 
  #define IND_R_PIN   ADC1_CHANNEL_7  // Pin 35 => Right Indicator Sense (Reserved)
  #endif
  
  #if PCB_V9 
  #include <SimpleButton.h>
  using namespace simplebutton;
  #define MODE_PIN	  19  // Pin 19 +> Mode selection
  #define HORN_PIN    39  // Pin 39 => Horn Sense
  #define IND_L_PIN   34  // Pin 34 => Left Indicator 
  #define IND_R_PIN   35  // Pin 35 => Right Indicator 
  Button* Horn_Button = NULL;
  Button* Ind_L_Button = NULL;
  Button* Ind_R_Button = NULL;
  Button* Mode_Button = NULL;
  #endif

  #define DEBOUNCE_TIME 100

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
#if PCB_V9 
  String drlState;
  String hornState;
  bool uberDisp;
  bool lyftDisp;

#endif

//LED Strip
  #include <Adafruit_NeoPixel.h>

//LED Controller Section
  #define NUM_LEDS  134   //161 leds is the lenght of the hood weather strip, 36 for the COB strip
  #define NUM_LEDS_HALF   (NUM_LEDS - 1) / 2    //Subtract 1 to calculate indexes
  #define LEDS_PER_PIXEL 1
  #define NUM_PIXELS (NUM_LEDS / LEDS_PER_PIXEL)
  #define NUM_PIXELS_HALF (NUM_PIXELS / 2)
  #define NUM_PIXELS_THIRD (NUM_PIXELS / 3)
  #define NUM_PIXELS_QUARTER (NUM_PIXELS / 4)
  #define NUM_PIXELS_FIFTH (NUM_PIXELS / 5)
  #define RGBW_STRIP false //for RGB Strips change to false
  #define RGBW_COLOR_ORDER NEO_GRBW //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  #define RGB_COLOR_ORDER NEO_RGB //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  #define FLASH_RATE int((70/120)*1000) //how many indicator flashes per minute as specifed by car manufacturer 
  #define msIND_DELAY  int(FLASH_RATE / NUM_PIXELS_HALF * 2 + 0.5) //Number of ms Indicator LED stays on for.

  
  #if RGBW_STRIP 
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, RGBW_COLOR_ORDER + NEO_KHZ800);
  //color definitions values, are expressed in rgbw format
  #define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
  #define DEFAULT_COLOR   leds.Color( 255, 197, 143, 255 )     //White (Adjusted to match vehicle oem light color)
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
  #define DEFAULT_COLOR   leds.Color( 241, 255, 131 )     //White (Adjusted to match vehicle oem light color)
  #define LYFT_COLOR      leds.Color( 255, 0,   191 )     //Magenta
  #define UBER_COLOR      leds.Color( 0,  255,  92  )     //Seafoam Green
  #define BRIGHTCOLOR   leds.Color( 241,  255,  131 )     //Full White
  #define DIMCOLOR      leds.Color(  48,  51,   27  )     //Dim White
  #define OFFCOLOR      leds.Color(   0,   0,   0   )     //Off
  #endif

#if LED_MATRIX    
//LED Matrix Panel
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

//MatrixPanel_I2S_DMA dma_display;
  MatrixPanel_I2S_DMA *dma_display = nullptr;
//LED Martrix pin section

//This is configured using a P2 64x64 LED Matrix, which has an E pin.
//Pinout for LED Matrix Controller V8, V9 or Yves Version
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
	  int btnPress = 0;

    void Increment () {
      Mode_Button->update();
      //if (Mode_Button->getState()){	btnPress++;}
			if (Mode_Button->clicked(DEBOUNCE_TIME)){	btnPress++;}
			if (btnPress > NUM_MODES) { btnPress = 0; }
		
      switch (btnPress) {
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
              uberDisp = true;
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
              lyftDisp = true;
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
            uberDisp = false;
            lyftDisp = false;
            break;
      }
    }

    void Init() {
      curColor = DEFAULT_COLOR;
      txtColor = "WHIT";
      delay(50);
      //Mode_Button->update();
    }
};
cModes curMode;

#if PCB_V9 == false
#include <TaskLCD.h>
#endif

#if PCB_V9 
#include <TaskLCD_V9.h>
#endif

#if PCB_V9 == false //V8 Boards require a different order of variables
String drlState;
String hornState;
#endif

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

void right_indicator(){
  if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
      for (int i = NUM_PIXELS_THIRD; i >= 0; i--){
        leds.setPixelColor(i, ANGRY_COLOR);
        delay(msIND_DELAY);
        leds.show();
      }
} 

void left_indicator(){
  if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
      for (int o = NUM_PIXELS - NUM_PIXELS_THIRD ; o <= NUM_PIXELS; o++){
        leds.setPixelColor(o, ANGRY_COLOR);
        delay(msIND_DELAY);
        leds.show();
      }
}

void hazard_indicator(){
    if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow hazard lights to work
    int ledLeft = 0; int ledRight = 0;
      for (int p = 1; p <= NUM_PIXELS_QUARTER; p++) {
        ledLeft = NUM_PIXELS_QUARTER - p;
        ledRight = NUM_PIXELS - NUM_PIXELS_QUARTER + p;
        leds.setPixelColor(ledRight, ANGRY_COLOR);              leds.setPixelColor(ledLeft, ANGRY_COLOR);
        delay(msIND_DELAY);
        leds.show();
      }
}

enum indState {
	OFF = 0,
  RIGHT,
	LEFT,
	HAZARD
	};

enum indState indStatus = OFF;

void indicator_function(){
  switch (indStatus)
  {
  case 1:
    right_indicator();
    break;
  case 2:
    left_indicator();
    break;
  case 3:
    hazard_indicator();
    break;  
  default:
    indStatus = OFF;
    break;
  }
}

void drl_mon(){
  static bool   RelayPin1State = false;
  if (curDRL > LO_VOLT && curDRL < (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      } 
      drlState = "LOW ";
      leds.setBrightness(MIN_BRIGHTNESS);
      if(LED_MATRIX) dma_display->setBrightness8(MIN_BRIGHTNESS);
  } else if (curDRL > (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      drlState = "HIGH";
      leds.setBrightness(MAX_BRIGHTNESS);
      if(LED_MATRIX) dma_display->setBrightness8(MAX_BRIGHTNESS);
  } else if (curDRL < VOLT_BUF){
      if ((uberDisp && curDRL < VOLT_BUF) || (lyftDisp && curDRL < VOLT_BUF)) {leds.setBrightness(MAX_BRIGHTNESS);} //Keep strip on when DRL are off in UBER/LYFT modes
      else {
        if (!RelayPin1State) { //turn off Relay 1
          RelayPin1State = false;
          digitalWrite(RELAY_PIN_1, RELAY_OFF);
        }
        drlState = "OFF ";
        leds.setBrightness(0); 
        }
    }
}

void button_function(){
  // MUST call the update() function first
  Horn_Button->update();
  //Mode_Button->update();
  Ind_L_Button->update();
  Ind_R_Button->update();
  bool currentHornButtonState = Horn_Button->getState();
  bool currentInd_LButtonState = Ind_L_Button->getState();
  bool currentInd_RButtonState = Ind_R_Button->getState();

    if (!currentHornButtonState) {
      leds.fill(ANGRY_COLOR);
      hornState = "BEEP";
    }
    else if (!currentInd_RButtonState && !currentInd_LButtonState) { 
      indStatus = HAZARD;
      }
    else if (!currentInd_RButtonState && currentInd_LButtonState) { 
      indStatus = RIGHT;
      } 
    else if (!currentInd_LButtonState && currentInd_RButtonState) { 
      indStatus = LEFT; 
      }
    else {
    leds.fill(curMode.curColor);
    hornState = "OFF ";
    indStatus = OFF;
    }
}

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
  if (PCB_V9) pinMode(HORN_PIN, INPUT);
  if (PCB_V9) pinMode(IND_L_PIN, INPUT);
  if (PCB_V9) pinMode(IND_R_PIN, INPUT);
  if (LEFT_IND) pinMode(IND_L_PIN, INPUT);
  if (RIGHT_IND) pinMode(IND_R_PIN, INPUT);

  //setup the buttons
#if PCB_V9 
  Horn_Button = new Button(HORN_PIN, true); //this is for an inverted setup where HIGH is the idle state of the buttons
  Mode_Button = new ButtonPullup(MODE_PIN);
  Ind_L_Button = new Button(IND_L_PIN, true);
  Ind_R_Button = new Button(IND_R_PIN, true);
#endif

  // Configure ADC
  if (DEBUG) {Serial.println("Start Init ADC");}
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  if (DEBUG) {Serial.println("Init ADC 1");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_11));
#if PCB_V9 == false
  if (DEBUG) {Serial.println("Init ADC 2");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(HORN_PIN, ADC_ATTEN_DB_11));
  //delay(150); // see if timing has effect on crashing
#endif

#if PCB_V9 == false
  if (RIGHT_IND) {
    if (DEBUG) {Serial.println("Init ADC 3");} 
    ESP_ERROR_CHECK(adc1_config_channel_atten(IND_R_PIN, ADC_ATTEN_DB_11));
    //delay(150); // see if timing has effect on crashing
  }
#endif

#if PCB_V9 == false
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

    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::FM6124;

    // Display Setup
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(MAX_BRIGHTNESS); //5-255 led matrix does not display using values lower than 5
    dma_display->clearScreen();
    dma_display->fillScreen(dma_display->color565(255, 255, 255));
#endif

  String drlState;
  String hornState;

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
#if PCB_V9 == false
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

#if PCB_V9 == false
  if (LEFT_IND) curInd_L *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
  if (RIGHT_IND) curInd_R *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
#endif

  drl_mon();

#if PCB_V9 == false
    if (curHorn > VOLT_BUF) leds.fill(ANGRY_COLOR); 
#endif
if (DEBUG) Serial.print("Mode Select Button State:");  Serial.println(Mode_Button->getState());
#if PCB_V9 
    button_function();
#endif 

    leds.show(); 
    indicator_function();
    curSample = 1;
    curDRL = 0;
    curHorn = 0;
    if (LEFT_IND) curInd_L = 0;
    if (RIGHT_IND) curInd_R = 0;
  }
}
