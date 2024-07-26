#include <TaskLCD_V9.h>
#include <main.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

extern unsigned long L1previousTime = 0;
extern unsigned long L2previousTime = 0;

void taskLCDUpdates( void * pvParameters) {
  //if(DEBUG) Serial.println("Start of LCD Function");
  unsigned long L1currentTime = millis();
  char tmpMessage[16];
  if ((L1currentTime - L1previousTime >= LCD_UPDATE_INTERVAL)) {  
    L1previousTime = L1currentTime; // Remember the time  
    lcd.clear();    //clear the display and home the cursor
    sprintf(tmpMessage, "MODE  DRL  Horn "); 
    lcd.setCursor(0,0); //move cursor to 1st line on display
    lcd.print(tmpMessage);
    //delay(LCD_UPDATE_INTERVAL);
    //if(DEBUG) Serial.println(String(L1currentTime) + "ms Line 1 Time");
  }
  

   while(true){
    //bool currentHornButtonState = Horn_Button->getState();
    //bool currentInd_LButtonState = Ind_L_Button->getState();
    //bool currentInd_RButtonState = Ind_R_Button->getState();
    unsigned long L2currentTime = millis();
#if SHOW_VOLTAGES_LCD    
    if (curVehicle.hornState == "BEEP") {
      sprintf(tmpMessage, "HORN  %04.1fV  %s", curVehicle.curDRL, curVehicle.hornState); 
    } else if (curVehicle.indStatus == curVehicle.LEFT) {
      sprintf(tmpMessage, "LEFT  %04.1fV  %s", curVehicle.curDRL, curVehicle.hornState); 
    } else if (curVehicle.indStatus == curVehicle.RIGHT) {
      sprintf(tmpMessage, "RGHT  %04.1fV  %s", curVehicle.curDRL, curVehicle.hornState); 
    } else if (curVehicle.indStatus == curVehicle.HAZARD) {
      sprintf(tmpMessage, "HZRD  %04.1fV  %s", curVehicle.curDRL, curVehicle.hornState); 
    } else {
      sprintf(tmpMessage, "%s  %04.1fV  %s", curMode.txtColor, curVehicle.curDRL, curVehicle.hornState);
    }
#endif  
#if SHOW_VOLTAGES_LCD == false
    if (curVehicle.hornState == "BEEP") {
      sprintf(tmpMessage, "HORN  %s  %s ", curVehicle.drlState, curVehicle.hornState); 
    } else if (curVehicle.indStatus == LEFT) {
      sprintf(tmpMessage, "LEFT  %s  %s ", curVehicle.drlState, curVehicle.hornState); 
    } else if (curVehicle.indStatus == RIGHT) {
      sprintf(tmpMessage, "RGHT  %s  %s ", curVehicle.drlState, curVehicle.hornState); 
    } else if (curVehicle.indStatus == HAZARD) {
      sprintf(tmpMessage, "HZRD  %s  %s ", curVehicle.drlState, curVehicle.hornState); 
    } else {
      sprintf(tmpMessage, "%s  %s  %s", curMode.txtColor, curVehicle.drlState, curVehicle.hornState);
    }
#endif  
    if ((L2currentTime - L2previousTime > LCD_UPDATE_INTERVAL)) {
      L2previousTime = L2currentTime; // Remember the time
      lcd.setCursor(0,1); //move cursor to 2nd line on display
      lcd.print(tmpMessage);
      //delay(LCD_UPDATE_INTERVAL);
      //if(DEBUG) Serial.println(String(L2currentTime) + "ms Line 2 Time");
      }
    }
    //if(DEBUG) Serial.println("End of LCD Function");
  }

void initTaskLCD() {
    // Create task on Core 1 to Update LCD
  xTaskCreatePinnedToCore(
    taskLCDUpdates,   // Function to implement the task 
    "taskLCDUpdates", // Name of the task 
    10000,            // Stack size in words 
    NULL,             // Task input parameter 
    0,                // Priority of the task 
    NULL,             //Task handle. 
    1);               //Core where the task should run 
}


//#ifdef __cplusplus
//}
//#endif
//#endif // _TASKLCD_V9_H_
