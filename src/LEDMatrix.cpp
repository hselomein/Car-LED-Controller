// #if LED_MATRIX 
//   // LED MATRIX Module configuration
//     HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
//     HUB75_I2S_CFG mxconfig(
//       PANEL_RES_X,  // module width
//       PANEL_RES_Y,  // module height
//       PANEL_CHAIN,   // Chain length
//       _pins // pin mapping
//     );

//     mxconfig.clkphase = false;
//     mxconfig.driver = HUB75_I2S_CFG::FM6124;

//     // Display Setup
//     dma_display = new MatrixPanel_I2S_DMA(mxconfig);
//     dma_display->begin();
//     dma_display->setBrightness8(MAX_BRIGHTNESS); //5-255 led matrix does not display using values lower than 5
//     dma_display->clearScreen();
//     dma_display->fillScreen(dma_display->color565(255, 255, 255));
// #endif


#if LED_MATRIX 
void screentest() {
  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
  delay(100);
  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(100);
  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width() - 1, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width() - 1, 0, 0, dma_display->height() - 1, dma_display->color444(15, 0, 0));
  delay(100);
  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  delay(100);
  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
  delay(100);
  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));
}
#endif