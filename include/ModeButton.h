
class cModes {
  public:
    Button* Mode_Button = NULL;
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
                curVehicle.uberDisp = true;
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
                curVehicle.lyftDisp = true;
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
            curVehicle.uberDisp = false;
            curVehicle.lyftDisp = false;
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
