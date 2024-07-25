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
#include <main.h>

//Declarations
bool firstLoop = true;

//-------------functions-----------------
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
    if(curVehicle.isHazardRunning) return; // Do not execute if hazard is running
    if (curVehicle.drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
    for (int i = NUM_PIXELS_THIRD; i >= 0; i--){
        leds.setPixelColor(i, ANGRY_COLOR);
        delay(msIND_DELAY);
        leds.show();
      }
    } 

void left_indicator(){
    if(curVehicle.isHazardRunning) return; // Do not execute if hazard is running
    if (curVehicle.drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow indicators to work
    for (int o = NUM_PIXELS - NUM_PIXELS_THIRD ; o <= NUM_PIXELS; o++){
        leds.setPixelColor(o, ANGRY_COLOR);
        delay(msIND_DELAY);
        leds.show();
      }
    }
  
void hazard_indicator(){
  curVehicle.isHazardRunning = true; // Set isHazardRunning to true when hazard starts
  if (curVehicle.drlState == "OFF ") leds.setBrightness(MAX_BRIGHTNESS); //if DRLs are off then allow hazard lights to work
  int ledLeft = 0; int ledRight = 0;
    for (int p = 1; p <= NUM_PIXELS_QUARTER; p++) {
      ledLeft = NUM_PIXELS_QUARTER - p;
      ledRight = NUM_PIXELS - NUM_PIXELS_QUARTER + p;
      leds.setPixelColor(ledRight, ANGRY_COLOR);              leds.setPixelColor(ledLeft, ANGRY_COLOR);
      delay(msIND_DELAY);
      leds.show();
    }
  curVehicle.isHazardRunning = false;  // After completing the hazard function, set isHazardRunning to false
  }

void indicator_function(){
  switch (curVehicle.indStatus)
  {
  case 1:
    curVehicle.currentIndicatorState = curVehicle.RIGHT;
    right_indicator();
    break;
  case 2:
    curVehicle.currentIndicatorState = curVehicle.LEFT;
    left_indicator();
    break;
  case 3:
    curVehicle.currentIndicatorState = curVehicle.HAZARD;
    hazard_indicator();
    break;  
  default:
    curVehicle.currentIndicatorState = curVehicle.OFF;
    curVehicle.indStatus = curVehicle.OFF;
    break;
  }
}
#endif

void drl_mon(){
  static bool RelayPin1State = false;
  if (curVehicle.curDRL > (LO_VOLT- VOLT_BUF) && curVehicle.curDRL < (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      } 
      curVehicle.drlState = "LOW ";
#if LED_STRIP
      leds.setBrightness(MIN_BRIGHTNESS);
#endif
#if LED_MATIRX
      dma_display->setBrightness8(MIN_BRIGHTNESS);
#endif
  } else if (curVehicle.curDRL > (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      curVehicle.drlState = "HIGH";
#if LED_STRIP
      leds.setBrightness(MAX_BRIGHTNESS);
#endif
#if LED_MATRIX
      dma_display->setBrightness8(MAX_BRIGHTNESS);
#endif
  } else if (curVehicle.curDRL < VOLT_BUF){
      if ((curVehicle.uberDisp && curVehicle.curDRL <  (LO_VOLT-VOLT_BUF)) || (curVehicle.lyftDisp && curVehicle.curDRL <  (LO_VOLT-VOLT_BUF))) { //Keep strip on when DRL are off in UBER/LYFT modes
#if LED_STRIP        
        leds.setBrightness(MAX_BRIGHTNESS);
#endif
        } 
      else {
        if (!RelayPin1State) { //turn off Relay 1
          RelayPin1State = false;
          digitalWrite(RELAY_PIN_1, RELAY_OFF);
        }
        curVehicle.drlState = "OFF ";
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
  Ind_L_Button->update();
  Ind_R_Button->update();
  bool currentHornButtonState = Horn_Button->getState();
  bool currentInd_LButtonState = !Ind_L_Button->getState();
  bool currentInd_RButtonState = !Ind_R_Button->getState();

  //bool currentInd_LButtonState = Ind_L_Button->clicked(DEBOUNCE_TIME);
  //bool currentInd_RButtonState = Ind_R_Button->clicked(DEBOUNCE_TIME);
  

    if (!currentHornButtonState) {
      leds.fill(ANGRY_COLOR);
      curVehicle.hornState = "BEEP";
    }
    else if (currentInd_RButtonState && currentInd_LButtonState) { 
      curVehicle.indStatus = curVehicle.HAZARD;
      }
    else if (currentInd_LButtonState && !currentInd_RButtonState) { 
      curVehicle.indStatus = curVehicle.LEFT;
      } 
    else  if (currentInd_RButtonState && !currentInd_LButtonState) { 
      curVehicle.indStatus = curVehicle.RIGHT;
      }
    else {
    leds.fill(curMode.curColor);
    curVehicle.hornState = "OFF ";
    curVehicle.indStatus = curVehicle.OFF;
    }
}
#endif

#if LCD_DISPLAY
#include <TaskLCD_V9.h>
#endif

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

  //setup the buttons
  Horn_Button = new Button(HORN_PIN, true); //this is for an inverted setup where HIGH is the idle state of the buttons
  Mode_Button = new ButtonPullup(MODE_PIN);
  Ind_L_Button = new Button(IND_L_PIN, true);
  Ind_R_Button = new Button(IND_R_PIN, true);

  // Configure ADC
  if (DEBUG) {Serial.println("Start Init ADC");}
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  if (DEBUG) {Serial.println("Init ADC 1");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_12));

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
  curVehicle.curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics);
  //curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics) + (RESISTOR_DIODE_OFFSET * 2);
  curSample++;

  if (DEBUG) {
    Serial.print("Horn Voltage:"); Serial.println(curVehicle.curHorn);
    Serial.print("DRL Voltage:"); Serial.println(curVehicle.curDRL);
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
    curVehicle.curDRL *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
    //curDRL += curDRL* (RESISTOR_DIODE_OFFSET/12);
    //curDRL += (curDRL - GRND_OFFSET) * (RESISTOR_DIODE_OFFSET + GRND_OFFSET) / HI_VOLT - GRND_OFFSET;
    curVehicle.curDRL = -1.0 * GRND_OFFSET * curVehicle.curDRL * curVehicle.curDRL / (GRND_OFFSET + HI_VOLT) + (RESISTOR_DIODE_OFFSET * HI_VOLT + GRND_OFFSET) * curVehicle.curDRL - (RESISTOR_DIODE_OFFSET + GRND_OFFSET) * (RESISTOR_DIODE_OFFSET + GRND_OFFSET) - GRND_OFFSET;
  
    drl_mon();
if (DEBUG) Serial.print("Mode Select Button State:");  Serial.println(Mode_Button->getState());
#if LED_STRIP
    button_function();
    leds.show(); 
    indicator_function();
    
#endif    
    curSample = 1;
    curVehicle.curDRL = 0;
    curVehicle.curHorn = 0;

    }
}
