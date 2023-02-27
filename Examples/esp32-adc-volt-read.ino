#include "sdkconfig.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <driver/adc.h>
//INSTALL THIS libaray
#include <SimpleKalmanFilter.h>
////
void setup()
{
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);// using GPIO 33
  //
  xTaskCreatePinnedToCore( fReadBattery, "fReadBattery", 4000, NULL, 3, NULL, 1 );
  Serial.begin(115200);   // serial monitor for debugging
  int battv = 0; //Corey added this here
}

////
void fReadBattery( void * parameter )
{
  float adcValue = 0.0f;
  const float r1 = 47000.0f; // R1 in ohm, 47K
  const float r2 = 10000.0f; // R2 in ohm, 10k 
  float Vbatt = 0.0f;
  int printCount = 0;
  float vRefScale = (3.3f / 4096.0f) * ((r1 + r2) / r2);
  uint64_t TimePastKalman  = esp_timer_get_time(); // used by the Kalman filter UpdateProcessNoise, time since last kalman calculation
  SimpleKalmanFilter KF_ADC_b( 1.0f, 1.0f, .01f );
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 1000; //delay for mS
  for (;;)
  {
    adc1_get_raw(ADC1_CHANNEL_5); //read and discard
    adcValue = float( adc1_get_raw(ADC1_CHANNEL_5) ); //take a raw ADC reading
    KF_ADC_b.setProcessNoise( (esp_timer_get_time() - TimePastKalman) / 1000000.0f ); //get time, in microsecods, since last readings
    adcValue = KF_ADC_b.updateEstimate( adcValue ); // apply simple Kalman filter
    //adcValue = KF_ADC_b.updateEstimate( float( adc1_get_raw(ADC1_CHANNEL_5) ); // apply simple Kalman filter
    Vbatt = adcValue * vRefScale;
    printCount++;
    if ( printCount == 3 )
    {
      //log_i( "Vbatt %f", Vbatt );
      Serial.print("Vbatt: ");      Serial.println(Vbatt);
      printCount = 0;
    }

    TimePastKalman = esp_timer_get_time(); // time of update complete
    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    //log_i( "fReadBattery %d",  uxTaskGetStackHighWaterMark( NULL ) );
  }
  vTaskDelete( NULL );
}
//need help with this part below
void loop() {
  //How do you call a funtion with a void and a pointer parameter?
 }