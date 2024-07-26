#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

//Build Configuration Options
  #define DEBUG false      //Enable serial output for debug, change to "false" to disable
  #define SCREENTEST false  //To enable the boot up screen test, change to "true", to disable change to "false"
  #define LED_MATRIX false   //Set to "true" if you want to use a 64x64 LED Matrix, "false" to disable
  #define LED_STRIP false   //Set to "true" if you want to use the led strip, "false" to disable
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
  
//#ifndef
  //#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000

#endif //_DEFINITIONS_H
