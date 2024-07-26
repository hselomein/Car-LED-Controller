#ifndef _LOGOS_H_
#define _LOGOS_H_

#ifndef _ESP32_HUB75_MATRIXPANEL_I2S_DMA_H
    #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#endif

//UBER and LYFT LOGOS
extern const bool LYFT_LOGO[];

extern const bool UBER_LOGO[];

void drawLyftLogo(MatrixPanel_I2S_DMA *dma_display);

void drawUberLogo(MatrixPanel_I2S_DMA *dma_display);

#endif // _LOGOS_H_