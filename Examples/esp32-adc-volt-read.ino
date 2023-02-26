#include "sdkconfig.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <driver/adc.h>
//INSTALL THIS library, https://www.arduino.cc/reference/en/libraries/simplekalmanfilter/
#include <SimpleKalmanFilter.h>
////
void setup()
{
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);// using GPIO 33
  //
  xTaskCreatePinnedToCore( fReadBattery, "fReadBattery", 4000, NULL, 3, NULL, 1 );
}
////
void fReadBattery( void * parameter )
{
  float adcValue = 0.0f;

 // notice my resistos values difeer from your.

 // Use 10K for R@

  const float r1 = 50500.0f; // R1 in ohm, 50K
  const float r2 = 10000.0f; // R2 in ohm, 10k potentiometer
  float Vbatt = 0.0f;
  int printCount = 0;
  float vRefScale = (3.3f / 4096.0f) * ((r1 + r2) / r2);
  uint64_t TimePastKalman  = esp_timer_get_time(); // used by the Kalman filter UpdateProcessNoise, time since last kalman calculation
  SimpleKalmanFilter KF_ADC_b( 1.0f, 1.0f, .01f );
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 1000; //delay for mS
  for (;;)
  {
    adc1_get_raw(ADC1_CHANNEL_7); //read and discard
    adcValue = float( adc1_get_raw(ADC1_CHANNEL_7) ); //take a raw ADC reading
    KF_ADC_b.setProcessNoise( (esp_timer_get_time() - TimePastKalman) / 1000000.0f ); //get time, in microsecods, since last readings
    adcValue = KF_ADC_b.updateEstimate( adcValue ); // apply simple Kalman filter
    Vbatt = adcValue * vRefScale;
    xSemaphoreTake( sema_CalculatedVoltage, portMAX_DELAY );
    CalculatedVoltage = Vbatt;
    xSemaphoreGive( sema_CalculatedVoltage );
    printCount++;
    if ( printCount == 3 )
    {
      log_i( "Vbatt %f", Vbatt );
      printCount = 0;
    }

    TimePastKalman = esp_timer_get_time(); // time of update complete
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    //log_i( "fReadBattery %d",  uxTaskGetStackHighWaterMark( NULL ) );
  }
  vTaskDelete( NULL );
}
////////
void loop() { }