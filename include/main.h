#ifndef _MAIN_H_
#define _MAIN_H_

#include <WString.h>

//#include "definitions.h"

//Arduino Standard
#include <driver/adc.h>
#include <esp_adc_cal.h>

#include "driver/gpio.h"

//Simple Button config
#include <SimpleButton.h>
using namespace simplebutton;

static esp_adc_cal_characteristics_t ADC1_Characteristics;

#if LED_MATRIX    
//LED Matrix Panel
    #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
    #include <logos.h>

    extern MatrixPanel_I2S_DMA *dma_display;
#endif

#endif