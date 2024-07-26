/*-----------------------------------------------------------------
    Program:        car_led_controller
    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, and button
		                activity. This is for the esp32 38pin device
    Date:           2/27/2024  
    Version:        .95b
    Authors:        Yves Avady
    Contriburtors:   Corey Davis, Jim Edmonds 
-----------------------------------------------------------------*/


#include <definitions.h>
#include <main.h>

//Create Objects
#include <vehicle.h>
vehicle curVehicle;
#if LED_MATRIX    
    MatrixPanel_I2S_DMA *dma_display = nullptr;
#endif

#include <ModeButton.h>
cModes curMode;






void drl_mon(){
  static bool RelayPin1State = false;
  if (curVehicle.curDRL > (LO_VOLT- VOLT_BUF) && curVehicle.curDRL < (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      } 
      curVehicle.drlState = "LOW ";
#if LED_STRIP
      leds.setBrightness(MIN_BRIGHTNESS);
#endif
#if LED_MATIRX
      dma_display->setBrightness8(MIN_BRIGHTNESS);
#endif
  } else if (curVehicle.curDRL > (HI_VOLT - VOLT_BUF)){
      if (!RelayPin1State) { //turn on Relay 1
        RelayPin1State = true;
        digitalWrite(RELAY_PIN_1, RELAY_ON);
      }
      curVehicle.drlState = "HIGH";
#if LED_STRIP
      leds.setBrightness(MAX_BRIGHTNESS);
#endif
#if LED_MATRIX
      dma_display->setBrightness8(MAX_BRIGHTNESS);
#endif
  } else if (curVehicle.curDRL < VOLT_BUF){
      if ((curVehicle.uberDisp && curVehicle.curDRL <  (LO_VOLT-VOLT_BUF)) || (curVehicle.lyftDisp && curVehicle.curDRL <  (LO_VOLT-VOLT_BUF))) { //Keep strip on when DRL are off in UBER/LYFT modes
#if LED_STRIP        
        leds.setBrightness(MAX_BRIGHTNESS);
#endif
        } 
      else {
        if (!RelayPin1State) { //turn off Relay 1
          RelayPin1State = false;
          digitalWrite(RELAY_PIN_1, RELAY_OFF);
        }
        curVehicle.drlState = "OFF ";
#if LED_STRIP
        if (LED_STRIP) leds.setBrightness(0); 
#endif
        }
    }
}

void setup()
{
  //Start LED Strip - Must be first to prevent delay


  //delay(250); // power-up safety delay use for bad powersupplies, enable only if needed

  if (DEBUG) Serial.begin(115200);   // serial monitor for debugging

  //Start Task LCD

  // Set pins as an input or output pin
  pinMode(RELAY_PIN_1, OUTPUT);
  if (LED_STRIP) pinMode(LED_PIN, OUTPUT);
  pinMode(DRL_PIN, INPUT);
  pinMode(HORN_PIN, INPUT);
  pinMode(IND_L_PIN, INPUT);
  pinMode(IND_R_PIN, INPUT);

  //setup the buttons
  curMode.Mode_Button = new ButtonPullup(MODE_PIN);
  curVehicle.Horn_Button = new Button(HORN_PIN, true); //this is for an inverted setup where HIGH is the idle state of the buttons
  curVehicle.Ind_L_Button = new Button(IND_L_PIN, true);
  curVehicle.Ind_R_Button = new Button(IND_R_PIN, true);

  // Configure ADC
  if (DEBUG) {Serial.println("Start Init ADC");}
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 0, &ADC1_Characteristics);
  ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
  if (DEBUG) {Serial.println("Init ADC 1");}
  ESP_ERROR_CHECK(adc1_config_channel_atten(DRL_PIN, ADC_ATTEN_DB_12));

  //Start LED Matrix

  // Initiate startup lighting sequence

#if (LED_MATRIX && SCREENTEST) 
  screentest();
#endif

  //Init TaskLCD
}

bool firstLoop = true;

void loop()
{
  static int    curSample = 1;
  curVehicle.curDRL += esp_adc_cal_raw_to_voltage(adc1_get_raw(DRL_PIN), &ADC1_Characteristics);
  curSample++;

  if (DEBUG) {
    Serial.print("Horn Voltage:"); Serial.println(curVehicle.curHorn);
    Serial.print("DRL Voltage:"); Serial.println(curVehicle.curDRL);
    Serial.print("Current Sample Number:"); Serial.println(curSample);
  }

  if (firstLoop) {
    curMode.Init();
    firstLoop = false;
  } else {
    curMode.Increment();
  }

  if (curSample > NUM_SAMPLES){
    // Adjust voltages
    curVehicle.curDRL *= VOLT_DIV_FACTOR / NUM_SAMPLES / 1000;
    //curDRL += curDRL* (RESISTOR_DIODE_OFFSET/12);
    //curDRL += (curDRL - GRND_OFFSET) * (RESISTOR_DIODE_OFFSET + GRND_OFFSET) / HI_VOLT - GRND_OFFSET;
    curVehicle.curDRL = -1.0 * GRND_OFFSET * curVehicle.curDRL * curVehicle.curDRL / (GRND_OFFSET + HI_VOLT) + (RESISTOR_DIODE_OFFSET * HI_VOLT + GRND_OFFSET) * curVehicle.curDRL - (RESISTOR_DIODE_OFFSET + GRND_OFFSET) * (RESISTOR_DIODE_OFFSET + GRND_OFFSET) - GRND_OFFSET;
  
    drl_mon();
    
    if (DEBUG) Serial.print("Mode Select Button State:");  Serial.println(curMode.Mode_Button->getState());

    //Perform LED Strip Tasks

    curSample = 1;
    curVehicle.curDRL = 0;
    curVehicle.curHorn = 0;
  }
}
