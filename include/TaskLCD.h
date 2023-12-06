#ifndef _TASKLCD_H_
#define _TASKLCD_H_

#include <stdio.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// set the LCD address to 0x27 for a 16 chars and 2 line display
#define LCD_COLS  16
#define LCD_ROWS  2 
#define LCD_UPDATE_INTERVAL 150   // How fast to update LCD in ms
hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip

float BUF = VOLT_BUF;
float HORN = curHorn;
float DRL = curDRL;

//void taskLCDUpdates( void * pvParameters );
void taskLCDUpdates( void * pvParameters) {
  char tmpMessage[16];
  lcd.clear();    //clear the display and home the cursor
  sprintf(tmpMessage, "MODE DRL   Horn "); 
  lcd.setCursor(0,0); //move cursor to 1st line on display
  lcd.print(tmpMessage);
  delay(50); 

  while(true){
    bool currentHornButtonState = Horn_Button->getState();
    bool currentInd_LButtonState = Ind_L_Button->getState();
    bool currentInd_RButtonState = Ind_R_Button->getState();
    if (!currentHornButtonState) {
      sprintf(tmpMessage, "HORN %04.1fV %04.1fV", DRL, HORN); 
    } else if (!currentInd_LButtonState && currentInd_RButtonState) {
      sprintf(tmpMessage, "LEFT %04.1fV %04.1fV", DRL, HORN); 
    } else if (!currentInd_RButtonState && currentInd_LButtonState) {
      sprintf(tmpMessage, "RGHT %04.1fV %04.1fV", DRL, HORN); 
    } else if (!currentInd_RButtonState && !currentInd_LButtonState ) {
      sprintf(tmpMessage, "HZRD %04.1fV %04.1fV", DRL, HORN); 
    } else {
      sprintf(tmpMessage, "%s %04.1fV %04.1fV", curMode.txtColor, DRL, HORN);
    }
    lcd.setCursor(0,1); //move cursor to 2nd line on display
    lcd.print(tmpMessage);
    delay(LCD_UPDATE_INTERVAL);
  }
}
//void initTaskLCD();
void initTaskLCD() {
    // Create task on Core 1 to Update LCD
  xTaskCreatePinnedToCore(
    taskLCDUpdates,   /* Function to implement the task */
    "taskLCDUpdates", /* Name of the task */
    10000,            /* Stack size in words */
    NULL,             /* Task input parameter */
    0,                /* Priority of the task */
    NULL,             /* Task handle. */
    1);               /* Core where the task should run */

}

#endif // _TASKLCD_H_