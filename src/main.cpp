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
#include <definitions.h>

//Arduino Standard
  #include <driver/adc.h>
  #include <esp_adc_cal.h>

//Simple Button config
  #include <SimpleButton.h>
  using namespace simplebutton;
  Button* Horn_Button = NULL;
  Button* Ind_L_Button = NULL;
  //Button* Ind_R_Button = NULL;
  Button* Mode_Button = NULL;

  static esp_adc_cal_characteristics_t ADC1_Characteristics;
  


#if LED_MATRIX    
//LED Matrix Panel
  #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

//MatrixPanel_I2S_DMA dma_display;
  MatrixPanel_I2S_DMA *dma_display = nullptr;
  #include <logos.h>
#endif



// Startup Configuration (Constants)
  static float curDRL   = 0.0f;
  static float curHorn  = 0.0f;
  static float curInd_L = 0.0f;
  static float curInd_R = 0.0f;
  String drlState;
  String hornState;
  bool uberDisp;
  bool lyftDisp;
  bool volatile l_ind_active = false;
  bool volatile r_ind_active = false;
  bool volatile hazard_active = false;


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
      if (Mode_Button->clicked(DEBOUNCE_TIME)){	btnPress++;}
			if (btnPress > NUM_MODES) { btnPress = 0; }
		
      switch (btnPress) {
        case 1:
            if (isLyftDisplayed == true) {isLyftDisplayed == false;}
#if LED_STRIP
            curColor = UBER_COLOR;
#endif
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
#if LED_STRIP
            curColor = LYFT_COLOR;
#endif
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
#if LED_STRIP        
            curColor = DEFAULT_COLOR;
#endif
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
#if LED_STRIP
      curColor = DEFAULT_COLOR;
#endif
      txtColor = "WHIT";
      //delay(50);
      //Mode_Button->update();
    }
};
cModes curMode;

#if LCD_DISPLAY
#include <TaskLCD_V9.h>
#endif

bool firstLoop = true;

#if LED_STRIP
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
#endif

#if LED_STRIP
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
#endif

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
#if LED_STRIP
void right_indicator(){
  if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
  for (int i = NUM_PIXELS_THIRD; i >= 0; i--){
      leds.setPixelColor(i, ANGRY_COLOR);
      delay(msIND_DELAY);
      leds.show();
    }
  r_ind_active = false;
} 


void left_indicator(){
  if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
  for (int o = NUM_PIXELS - NUM_PIXELS_THIRD ; o <= NUM_PIXELS; o++){
      leds.setPixelColor(o, ANGRY_COLOR);
      delay(msIND_DELAY);
      //if(startTime - millis() > msDELAY){
      leds.show();
      //}
      }
  l_ind_active = false;
}

void hazard_indicator(){
  unsigned long startTime = millis();
  if (drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow hazard lights to work
  int ledLeft = 0; int ledRight = 0;
    for (int p = 1; p <= NUM_PIXELS_QUARTER; p++) {
      ledLeft = NUM_PIXELS_QUARTER - p;
      ledRight = NUM_PIXELS - NUM_PIXELS_QUARTER + p;
      leds.setPixelColor(ledRight, ANGRY_COLOR);              leds.setPixelColor(ledLeft, ANGRY_COLOR);
      delay(msIND_DELAY);
      //if(startTime - millis() > msDELAY){
      leds.show();
      //}
    }
}
#endif

#if LED_STRIP
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
    //right_indicator();
    r_ind_active = false;
    break;
  case 2:
    //left_indicator();
    l_ind_active = false;
    break;
  case 3:
    //hazard_indicator();
    hazard_active = false;
    break;  
  default:
    indStatus = OFF;
    break;
  }
}
#endif

void IRAM_ATTR right_indicator_isr(){
  r_ind_active = true;
  indStatus = RIGHT;
} 

void IRAM_ATTR left_indicator_isr(){
  l_ind_active = true;
  indStatus = LEFT;
} 

void drl_mon(){
  static bool RelayPin1State = false;
  if (curDRL > LO_VOLT && curDRL < (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      } 
      drlState = "LOW ";
#if LED_STRIP
      leds.setBrightness(MIN_BRIGHTNESS);
#endif
#if LED_MATIRX
      dma_display->setBrightness8(MIN_BRIGHTNESS);
#endif
  } else if (curDRL > (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      drlState = "HIGH";
#if LED_STRIP
      leds.setBrightness(MAX_BRIGHTNESS);
#endif
#if LED_MATRIX
      dma_display->setBrightness8(MAX_BRIGHTNESS);
#endif
  } else if (curDRL < VOLT_BUF){
      if ((uberDisp && curDRL < VOLT_BUF) || (lyftDisp && curDRL < VOLT_BUF)) { //Keep strip on when DRL are off in UBER/LYFT modes
#if LED_STRIP        
        leds.setBrightness(MAX_BRIGHTNESS);
#endif
        } 
      else {
        if (!RelayPin1State) { //turn off Relay 1
          RelayPin1State = false;
          digitalWrite(RELAY_PIN_1, RELAY_OFF);
        }
        drlState = "OFF ";
#if LED_STRIP
        if (LED_STRIP) leds.setBrightness(0); 
#endif
        }
    }
}

#if LED_STRIP
void button_function(){
  // MUST call the update() function first
  Horn_Button->update();
  //Ind_L_Button->update();
  //Ind_R_Button->update();
  bool currentHornButtonState = Horn_Button->getState();
  //bool currentInd_LButtonState = Ind_L_Button->getState();
  //bool currentInd_RButtonState = Ind_R_Button->getState();

    if (!currentHornButtonState) {
      leds.fill(ANGRY_COLOR);
      hornState = "BEEP";
    } else if(l_ind_active && r_ind_active){
      hazard_active = true;
      indStatus = HAZARD;
    }// else if (r_ind_active) {
      //r_ind_active = true;
    //} else if (l_ind_active) {
      //l_ind_active = true;
    //}
     else {
    leds.fill(curMode.curColor);
    hornState = "OFF ";
    indStatus = OFF;
    }
}
#endif

//void gpioHandler(void* arg);

//-----------main program-----------------

void setup()
{
  // Start LEDs
  digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
#if LED_STRIP
  leds.begin();
  leds.show();
  leds.setBrightness(MAX_BRIGHTNESS);
#endif

  //delay(250); // power-up safety delay use for bad powersupplies, enable only if needed
  
  if (DEBUG) Serial.begin(115200);   // serial monitor for debugging

#if LCD_DISPLAY    
  // set up the LCD:
  lcd.begin(LCD_COLS, LCD_ROWS); //begin() will automatically turn on the backlight
  lcd.clear();            //clear the display  
  lcd.home();             //move cursor to 1st line on display
  lcd.print("LOADING");   
  lcd.setCursor(0,1);     //move cursor to 2nd line on display
  lcd.print("PLEASE WAIT");   
#endif  


  // Set pins as an input or output pin
  pinMode(RELAY_PIN_1, OUTPUT);
  if (LED_STRIP) pinMode(LED_PIN, OUTPUT);
  pinMode(DRL_PIN, INPUT);
  pinMode(HORN_PIN, INPUT);
  pinMode(IND_L_PIN, INPUT);
  pinMode(IND_R_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(IND_R_PIN), right_indicator_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(IND_L_PIN), left_indicator_isr, RISING);

  //setup the buttons
  Horn_Button = new Button(HORN_PIN, true); //this is for an inverted setup where HIGH is the idle state of the buttons
  Mode_Button = new ButtonPullup(MODE_PIN);
  //Ind_L_Button = new Button(IND_L_PIN, true);
  //Ind_R_Button = new Button(IND_R_PIN, true);

  // Configure ADC
  if (DEBUG) {Serial.println("Start Init ADC");}
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  if (DEBUG) {Serial.println("Init ADC 1");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_11));

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
#if LED_STRIP
  startupSequence(); 
#endif

#if LED_MATRIX 
  if (SCREENTEST) screentest();
#endif

#if LCD_DISPLAY
 initTaskLCD();
#endif


}

void loop()
{
  static int    curSample = 1;
  curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics);
  curSample++;
     
  //This is more responsive when using interrupts
  if(r_ind_active && l_ind_active){
      hazard_indicator();
      hazard_active = false;
  } else if (r_ind_active) {
      right_indicator();
      r_ind_active = false;
  } else if (l_ind_active) {
      left_indicator();
      l_ind_active = false;
  }
  

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
  
    drl_mon();
if (DEBUG) Serial.print("Mode Select Button State:");  Serial.println(Mode_Button->getState());
#if LED_STRIP
    button_function();
    leds.show(); 
    indicator_function();    
   
#endif    
    curSample = 1;
    curDRL = 0;
    curHorn = 0;
    }
}
