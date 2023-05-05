
// Example sketch which shows how to display some patterns
// on a 64x64 LED matrix
//

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"



#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 64  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1   // Total number of panels chained one to another

//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myMAGENTA = dma_display->color565(255, 0, 191);


void setup() {

  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,  // module width
    PANEL_RES_Y,  // module height
    PANEL_CHAIN   // Chain length
  );

  mxconfig.gpio.e = 32;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(5) //5-255 led matrix does not display using values lower than 5
  dma_display->clearScreen();
  dma_display->fillScreen(myWHITE);


  screentest();
}

void loop() {

  //

  // show uber and lyft signs
  drawUber();
  delay(1000);
  dma_display->fillScreen(myBLACK);
  delay(1000);
  drawLyft();
  delay(1000);
  dma_display->fillScreen(myBLACK);
  delay(1000);
}


void drawUber() {
  // Print the word UBER
  dma_display->clearScreen();
  // dma_display->setBrightness8(255); //0-255 90
  dma_display->fillScreen(myWHITE);
  dma_display->setTextSize(2);  // size 1 == 8 pixels high
  dma_display->setTextColor(myBLACK);
  dma_display->setCursor(9, 25);  // start at top left, with 8 pixel of spacing
  dma_display->println("UBER");
}

void drawLyft() {
  // Print the word LYFT
  dma_display->clearScreen();
  // dma_display->setBrightness8(255); //0-255 90
  dma_display->fillScreen(myMAGENTA);
  dma_display->setTextSize(2);    // size 1 == 8 pixels high
  dma_display->setCursor(9, 25);  // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myWHITE);
  dma_display->println("LYFT");
}

void screentest() {

  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
  delay(50);

  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(50);

  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width() - 1, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width() - 1, 0, 0, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  delay(50);

  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  delay(50);

  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
  delay(50);

  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));
}