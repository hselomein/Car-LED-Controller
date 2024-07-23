//Build Configuration Options
  #define DEBUG false      //Enable serial output for debug, change to "false" to disable
  #define SCREENTEST false  //To enable the boot up screen test, change to "true", to disable change to "false"
  #define LED_MATRIX true   //Set to "true" if you want to use a 64x64 LED Matrix, "false" to disable
  #define LED_STRIP true    //Set to "true" if you want to use the led strip, "false" to disable
  #define LCD_DISPLAY true   //Set to "true" if you want to use the LCD Character Display, "false" to disable
  #define NUM_MODES 2       //How many modes will the mode button handle (2 for Uber and Lyft signs)
  #define SHOW_VOLTAGES_LCD true //Set to "true" to display voltages on the LCD, "false" to disable

// Pins to device mapping
  #define RELAY_PIN_1 18                // Pin 18 => In1 Relay
  #define LED_PIN     23                // Pin 23 => LED Controller Signal
  #define DRL_PIN     ADC1_CHANNEL_0    // Pin 39 => DRL Sense

  #define MODE_PIN	  19  // Pin 19 +> Mode selection
  #define HORN_PIN    39  // Pin 39 => Horn Sense
  #define IND_L_PIN   34  // Pin 34 => Left Indicator 
  #define IND_R_PIN   35  // Pin 35 => Right Indicator 

  #define DEBOUNCE_TIME 100

  //Define lcd and led brightness
  #define MAX_BRIGHTNESS  255
  #define MIN_BRIGHTNESS  63
  #define MED_BRIGHTNESS  127

  #define RELAY_ON LOW
  #define RELAY_OFF HIGH

  #define NUM_SAMPLES     50              // number of analog samples to take per reading
  #define R1              47.0            // Resistor 1 value of voltage divider
  #define R2              10.0            // Resistor 2 value of voltage divider
  #define VOLT_DIV_FACTOR (R1+R2)/R2      //voltage divider factor
  #define RESISTOR_DIODE_OFFSET 0.3       //voltage drop across the diode
  #define GRND_OFFSET 0.8
  //static esp_adc_cal_characteristics_t ADC1_Characteristics;

  #define VOLT_BUF        1
  #define HI_VOLT         6
  #define LO_VOLT         2
  
// Startup Configuration (Constants)
  #define msDELAY  int(400 / NUM_PIXELS + 0.5)   //Number of ms LED stays on for.
  #define numLOOPS      4   //Number of passes over entire LED strip

#if LED_STRIP
//LED Strip
  #include <Adafruit_NeoPixel.h>

//LED Controller Section
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

  
#if RGBW_STRIP 
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, RGBW_COLOR_ORDER + NEO_KHZ800);
  //color definitions values, are expressed in rgbw format
  #define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
  #define DEFAULT_COLOR   leds.Color( 251, 255, 141, 255 )     //White (Adjusted to match vehicle oem light color)
  #define LYFT_COLOR      leds.Color( 255,  0, 191,  0 )     //Magenta
  #define UBER_COLOR      leds.Color( 0,  255,  92,  0 )     //Seafoam Green
  #define BRIGHTCOLOR   leds.Color( 255, 255, 255, 255 )     //Full White
  #define DIMCOLOR      leds.Color(  50,  50,  50,  50 )     //Dim White
  #define OFFCOLOR      leds.Color(   0,   0,   0,   0 )     //Off
  #endif

#if RGBW_STRIP == false
  Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, RGB_COLOR_ORDER + NEO_KHZ800);
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

#if LED_MATRIX 
//LED Martrix pin section
//This is configured using a P2 64x64 LED Matrix, which has an E pin.
//Pinout for LED Matrix Controller V8, V9 or Yves Version
  #define R1_PIN  25
  #define G1_PIN  26
  #define B1_PIN  33
  #define R2_PIN  14
  #define G2_PIN  12
  #define B2_PIN  13
  #define A_PIN   27
  #define B_PIN   2 
  #define C_PIN   5
  #define D_PIN   17
  #define E_PIN   32  
  #define LAT_PIN 4
  #define OE_PIN  15
  #define CLK_PIN 16
    
  //LED Matrix Initialization
  #define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
  #define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
  #define PANEL_CHAIN 1   // Total number of panels chained one to another
#endif

  #include "driver/gpio.h"


//#ifndef
  //#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000
