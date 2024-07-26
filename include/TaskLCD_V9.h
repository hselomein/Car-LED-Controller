#ifndef _TASKLCD_V9_H_
#define _TASKLCD_V9_H_


#include <stdio.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

//#ifdef __cplusplus
//extern "C" {
//#endif

//void taskLCDUpdates( void * pvParameters );
//void initTaskLCD();


// LCD column and row settings
#define LCD_COLS  16
#define LCD_ROWS  2 
#define LCD_UPDATE_INTERVAL 150   // How fast to update LCD in ms

class TaskLCD {
  public:
    void *taskLCDUpdates( void * pvParameters );
    void initTaskLCD();
    
    hd44780_I2Cexp lcd;               // Declare lcd object: auto locate & config exapander chip

};

extern TaskLCD curLCD;





#if LED_STRIP
//LED Strip
  #define NUM_LEDS  134   //161 leds is the lenght of the hood weather strip, 36 for the COB strip
  #define NUM_LEDS_HALF   (NUM_LEDS - 1) / 2    //Subtract 1 to calculate indexes
  #define LEDS_PER_PIXEL 1
  #define NUM_PIXELS (NUM_LEDS / LEDS_PER_PIXEL)
  #define NUM_PIXELS_HALF (NUM_PIXELS / 2)
  #define NUM_PIXELS_THIRD (NUM_PIXELS / 3)
  #define NUM_PIXELS_QUARTER (NUM_PIXELS / 4)
  #define NUM_PIXELS_FIFTH (NUM_PIXELS / 5)
  #define RGBW_STRIP false //for RGB Strips change to false
  #define RGBW_COLOR_ORDER NEO_GRBW //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  #define RGB_COLOR_ORDER NEO_RGB //Change this to match the order of color for the LED Strip see NeoPixel library for definitions
  #define FLASH_RATE int((80/120)*1000) //how many indicator flashes per minute as specifed by car manufacturer 
  #define msIND_DELAY  int(FLASH_RATE / NUM_PIXELS_HALF * 2 + 0.5) //Number of ms Indicator LED stays on for.
  #define msDELAY  int(400 / NUM_PIXELS + 0.5)   //Number of ms LED stays on for.
  #define numLOOPS      4   //Number of passes over entire LED strip

  #include <Adafruit_NeoPixel.h>

  #if RGBW_STRIP 
    #define cur_COLOR_ORDER RGBW_COLOR_ORDER
  #else
    #define cur_COLOR_ORDER RGB_COLOR_ORDER
  #endif

  extern Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, cur_COLOR_ORDER + NEO_KHZ800);

  #if RGBW_STRIP 
    //color definitions values, are expressed in rgbw format
    #define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
    #define DEFAULT_COLOR   leds.Color( 251, 255, 141, 255 )     //White (Adjusted to match vehicle oem light color)
    #define LYFT_COLOR      leds.Color( 255,  0, 191,  0 )     //Magenta
    #define UBER_COLOR      leds.Color( 0,  255,  92,  0 )     //Seafoam Green
    #define BRIGHTCOLOR   leds.Color( 255, 255, 255, 255 )     //Full White
    #define DIMCOLOR      leds.Color(  50,  50,  50,  50 )     //Dim White
    #define OFFCOLOR      leds.Color(   0,   0,   0,   0 )     //Off
  #else
    //color definitions values, are expressed in rgb format
    #define ANGRY_COLOR     leds.Color( 255, 60,  0   )     //Amber
    #define DEFAULT_COLOR   leds.Color( 251, 255, 141 )     //White (Adjusted to match vehicle oem light color)
    #define LYFT_COLOR      leds.Color( 255, 0,   191 )     //Magenta
    #define UBER_COLOR      leds.Color( 0,  255,  92  )     //Seafoam Green
    #define BRIGHTCOLOR   leds.Color( 251,  255,  141 )     //Full White
    #define DIMCOLOR      leds.Color( 125,  127,  70  )     //Dim White
    #define OFFCOLOR      leds.Color(   0,   0,   0   )     //Off
  #endif
#endif


#endif // _TASKLCD_V9_H_
