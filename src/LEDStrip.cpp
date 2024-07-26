 // Start LEDs
//   digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
// #if LED_STRIP
//   leds.begin();
//   leds.show();
//   leds.setBrightness(MAX_BRIGHTNESS);
// #endif

//Initialize Startup Lighting Sequence
#if LED_STRIP
  startupSequence(); 
#endif

#if LED_STRIP
    button_function();
    leds.show(); 
    indicator_function();
#endif    

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