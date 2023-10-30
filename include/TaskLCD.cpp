#include <TaskLCD.h>

void taskLCDUpdates( void * pvParameters) {
  char tmpMessage[16];
  lcd.clear();    //clear the display and home the cursor
  sprintf(tmpMessage, "MODE DRL   Horn "); 
  lcd.setCursor(0,0); //move cursor to 1st line on display
  lcd.print(tmpMessage);
  delay(50); 

  while(true){
    if (HORN > BUF) {
      sprintf(tmpMessage, "HORN %04.1fV %04.1fV", DRL, HORN); 
    } else {
      sprintf(tmpMessage, "%s %04.1fV %04.1fV", curMode.txtColor, DRL, HORN);  
    }
    lcd.setCursor(0,1); //move cursor to 2nd line on display
    lcd.print(tmpMessage);
    delay(LCD_UPDATE_INTERVAL);
  }
}

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