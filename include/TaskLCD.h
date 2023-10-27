#ifndef _TASKLCD_H_
#define _TASKLCD_H_

#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// set the LCD address to 0x27 for a 16 chars and 2 line display
#define LCD_COLS  16
#define LCD_ROWS  2 
#define LCD_UPDATE_INTERVAL 150   // How fast to update LCD in ms
hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip

void taskLCDUpdates( void * pvParameters );

void initTaskLCD();

#endif // _TASKLCD_H_