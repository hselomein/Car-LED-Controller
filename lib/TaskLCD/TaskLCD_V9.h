#ifndef _TASKLCD_V9_H_
#define _TASKLCD_V9_H_


#include <stdio.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

//#ifdef __cplusplus
//extern "C" {
//#endif

void taskLCDUpdates( void * pvParameters );
void initTaskLCD();

/*
// LCD column and row settings
#define LCD_COLS  16
#define LCD_ROWS  2 
hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip
#define LCD_UPDATE_INTERVAL 150   // How fast to update LCD in ms


extern unsigned long L1previousTime = 0;
extern unsigned long L2previousTime = 0;




void taskLCDUpdates( void * pvParameters) {
  if(DEBUG) Serial.println("Start of LCD Function");
  unsigned long L1currentTime = millis();
  char tmpMessage[16];
  if ((L1currentTime - L1previousTime >= LCD_UPDATE_INTERVAL)) {  
    L1previousTime = L1currentTime; // Remember the time  
    lcd.clear();    //clear the display and home the cursor
    sprintf(tmpMessage, "MODE  DRL  Horn "); 
    lcd.setCursor(0,0); //move cursor to 1st line on display
    lcd.print(tmpMessage);
    //delay(LCD_UPDATE_INTERVAL);
    if(DEBUG) Serial.println(String(L1currentTime) + "ms Line 1 Time");
  }
  

   while(true){
    //bool currentHornButtonState = Horn_Button->getState();
    //bool currentInd_LButtonState = Ind_L_Button->getState();
    //bool currentInd_RButtonState = Ind_R_Button->getState();
    unsigned long L2currentTime = millis();
#if SHOW_VOLTAGES_LCD    
    if (hornState == "BEEP") {
      sprintf(tmpMessage, "HORN  %04.1fV  %s", curDRL, hornState); 
    } else if (indStatus == LEFT) {
      sprintf(tmpMessage, "LEFT  %04.1fV  %s", curDRL, hornState); 
    } else if (indStatus == RIGHT) {
      sprintf(tmpMessage, "RGHT  %04.1fV  %s", curDRL, hornState); 
    } else if (indStatus == HAZARD) {
      sprintf(tmpMessage, "HZRD  %04.1fV  %s", curDRL, hornState); 
    } else {
      sprintf(tmpMessage, "%s  %04.1fV  %s", curMode.txtColor, curDRL, hornState);
    }
#endif  
#if SHOW_VOLTAGES_LCD == false
    if (hornState == "BEEP") {
      sprintf(tmpMessage, "HORN  %s  %s ", drlState, hornState); 
    } else if (indStatus == LEFT) {
      sprintf(tmpMessage, "LEFT  %s  %s ", drlState, hornState); 
    } else if (indStatus == RIGHT) {
      sprintf(tmpMessage, "RGHT  %s  %s ", drlState, hornState); 
    } else if (indStatus == HAZARD) {
      sprintf(tmpMessage, "HZRD  %s  %s ", drlState, hornState); 
    } else {
      sprintf(tmpMessage, "%s  %s  %s", curMode.txtColor, drlState, hornState);
    }
#endif  
    if ((L2currentTime - L2previousTime > LCD_UPDATE_INTERVAL)) {
      L2previousTime = L2currentTime; // Remember the time
      lcd.setCursor(0,1); //move cursor to 2nd line on display
      lcd.print(tmpMessage);
      //delay(LCD_UPDATE_INTERVAL);
      if(DEBUG) Serial.println(String(L2currentTime) + "ms Line 2 Time");
      }
    }
    if(DEBUG) Serial.println("End of LCD Function");
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

*/
//#ifdef __cplusplus
//}


//#endif
#endif // _TASKLCD_V9_H_
