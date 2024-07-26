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