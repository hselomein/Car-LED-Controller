#ifndef _TASKLCD_V9_H_
#define _TASKLCD_V9_H_


#include <stdio.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// LCD column and row settings
#define LCD_COLS  16
#define LCD_ROWS  2 
hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip
#define LCD_UPDATE_INTERVAL 150   // How fast to update LCD in ms


//void taskLCDUpdates( void * pvParameters );
//void initTaskLCD();

void taskLCDUpdates( void * pvParameters) {
  unsigned long startTime = millis();
  char tmpMessage[16];
  lcd.clear();    //clear the display and home the cursor
  sprintf(tmpMessage, "MODE DRL   Horn "); 
  lcd.setCursor(0,0); //move cursor to 1st line on display
if ((startTime - millis() > LCD_UPDATE_INTERVAL)) {
  lcd.print(tmpMessage);
  }
  
  while(true){
    bool currentHornButtonState = Horn_Button->getState();
    bool currentInd_LButtonState = Ind_L_Button->getState();
    bool currentInd_RButtonState = Ind_R_Button->getState();
    if (!currentHornButtonState) {
      sprintf(tmpMessage, "HORN %s %s", drlState, hornState); 
    } else if (!currentInd_LButtonState && currentInd_RButtonState) {
      sprintf(tmpMessage, "LEFT %s %s", drlState, hornState); 
    } else if (!currentInd_RButtonState && currentInd_LButtonState) {
      sprintf(tmpMessage, "RGHT %s %s", drlState, hornState); 
    } else if (!currentInd_RButtonState && !currentInd_LButtonState ) {
      sprintf(tmpMessage, "HZRD %s %s", drlState, hornState); 
    } else {
      sprintf(tmpMessage, "%s %s %s", curMode.txtColor, drlState, hornState);
    }
    if ((startTime - millis() > LCD_UPDATE_INTERVAL)) {
    lcd.setCursor(0,1); //move cursor to 2nd line on display
    lcd.print(tmpMessage);
    }
    if (DEBUG) Serial.println(String(millis() - startTime) + "ms");
    }
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

#endif // _TASKLCD_V9_H_
