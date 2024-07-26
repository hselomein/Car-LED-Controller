
cModes curMode;


//-------------functions-----------------

float Abs(float val) {
  if (val > 0) return val;
  else return -val;
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

