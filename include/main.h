#ifndef _MAIN_H_
#define _MAIN_H_

#include <WString.h>

#include "definitions.h"

//Arduino Standard
#include <driver/adc.h>
#include <esp_adc_cal.h>

//Simple Button config
#include <SimpleButton.h>
using namespace simplebutton;
Button* Horn_Button = NULL;
Button* Ind_L_Button = NULL;
Button* Ind_R_Button = NULL;
Button* Mode_Button = NULL;
static esp_adc_cal_characteristics_t ADC1_Characteristics;
  
#if LED_MATRIX    
//LED Matrix Panel
    #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
    #include <logos.h>

    MatrixPanel_I2S_DMA *dma_display = nullptr;
#endif

#include "driver/gpio.h"

class vehicle {
public:
// Startup Configuration (Constants)
    float curDRL   = 0.0f;
    float curHorn  = 0.0f;
    float curInd_L = 0.0f;
    float curInd_R = 0.0f;
    String drlState;
    String hornState;
    bool uberDisp;
    bool lyftDisp;
    bool isHazardRunning = false; // Global variable to track hazard function state

#if LED_STRIP
    enum indState {
	OFF = 0,
    RIGHT,
	LEFT,
	HAZARD
	};

    enum indState indStatus = OFF;
    indState currentIndicatorState = OFF; // Initialize with OFF or any default state
#endif
//private:
};
vehicle curVehicle;

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
cModes curMode;

#endif
