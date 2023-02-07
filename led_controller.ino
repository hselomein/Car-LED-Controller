
/*-----------------------------------------------------------------
    Program:        car_led_controller

    Description:    Controls LED strip behavior based on various 
                    Inputs such as, drl, headlights, horn, activity

    Date:           Added when finalized to production (2/6/2023)

    Author:         Corey Davis, Yves Avady

    Credits:        volt_measure by W.A Smith 
                    http://startingelectronics.org

                    Add credits for code obtained from elsewhere
                    

-----------------------------------------------------------------*/
#include <FastLED.h>

// Pins to device mapping
// D2 => In1 Relay (RelayPin1)
// D3 => In2 Relay (RelayPin2)
// D12 => LED Controller Signal (LED_Signal) 
// A2 => DRL Sense (DRL)
// A3 => PK_L Sense (PK_L)
// A4 => Hn Sense (HN)
// A5 => HiBm Sense (HIBM)

// Pins
int RelayPin1 = 2;
int RelayPin2 = 3;
int LED_Signal = 11;
int DRL = A5;
int PK_L = A4;
int NH = A3;
int HIBM = A2;
// Setup relay powerstates
// reverse if relay is wired differently
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// number of analog samples to take per reading
#define NUM_SAMPLES 10

//LED Controller Section
#define LED_PIN     11
#define NUM_LEDS    90
#define BRIGHTNESS  250
#define BRIGHTNESS_H 255
#define BRIGHTNESS_L 127
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG

CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

int sum_A2 = 0;
int sum_A3 = 0;
int sum_A4 = 0;
int sum_A5 = 0;
unsigned char sample_count = 0;                    // sum of samples taken
unsigned char sample_count_A2 = 0;
unsigned char sample_count_A3 = 0;
unsigned char sample_count_A4 = 0;
unsigned char sample_count_A5 = 0; // current sample number
float voltage_A2 = 0.0;
float voltage_A3 = 0.0;
float voltage_A4 = 0.0;
float voltage_A5 = 0.0;            // calculated voltage

void setup()
{
    Serial.begin(9600);
    // Set pins as an input or output pin
	  pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(LED_Signal, OUTPUT);
    pinMode(DRL, INPUT);
    pinMode(PK_L, INPUT);
    pinMode(NH, INPUT);
    pinMode(HIBM, INPUT);
	  //pinMode(RelayPin, OUTPUT);
    //digitalWrite(RelayPin, LOW);
    //Start LEDs
		delay(3000 ); // power-up safety delay
		FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
		FastLED.setBrightness(  BRIGHTNESS );
		//currentPalette = RainbowColors_p;
		//currentBlending = LINEARBLEND;

}

void loop()
{
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum_A5 += analogRead(A5);
        //sample_count_A5++;
		    sum_A4 += analogRead(A4);
        //sample_count_A4++;
		    sum_A3 += analogRead(A3);
        //sample_count_A3++;
		    sum_A2 += analogRead(A2);
        //sample_count_A2++;
        sample_count++;
        //delay(100);
    
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.09V is the calibrated reference voltage
    voltage_A5 = ((float)sum_A5 / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  voltage_A4 = ((float)sum_A4 / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  voltage_A3 = ((float)sum_A3 / (float)NUM_SAMPLES * 5.09) / 1024.0;
	  voltage_A2 = ((float)sum_A2 / (float)NUM_SAMPLES * 5.09) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 22 when using voltage divider that
    // divides by 22. 22.410 is the calibrated voltage divide
    // value
    
	  Serial.print("Voltage of A2 = ");
	  Serial.print(voltage_A2 * 22.410);
    Serial.println (" V");
	  Serial.print("Voltage of A3 = ");
	  Serial.print(voltage_A3 * 22.410);
    Serial.println (" V");
	  Serial.print("Voltage of A4 = ");
	  Serial.print(voltage_A4 * 22.410);
    Serial.println (" V");
	  Serial.print("Voltage of A5 = ");
	  Serial.print(voltage_A5 * 22.410);
    Serial.println (" V");
    sample_count = 0;
    sample_count_A2 = 0;
	  sample_count_A3 = 0;
	  sample_count_A4 = 0;
	  sample_count_A5 = 0;
    sum_A2 = 0;
	  sum_A3 = 0;
	  sum_A4 = 0;
	  sum_A5 = 0;
    //digitalWrite(RelayPin1, RELAY_ON);

    if ( voltage_A5 * 22.410 >= 1.00 &&  voltage_A5 * 22.410 <= 10.00) {  //set lower voltage to 2v, set to 1v due to limitation of testing hardware
      //turn on relay1
      Serial.print("Turning on relay1 because A5 is ");
	    Serial.print(voltage_A5 * 22.410);
      Serial.println (" V");      
      digitalWrite(RelayPin1, RELAY_ON);
      leds[NUM_LEDS] = CRGB(255,255,255); 
      FastLED.setBrightness( BRIGHTNESS_L ); //set half brightness
      FastLED.show();
      Serial.println("DRL Brightness level HALF");
      delay(500);
      }
	  if (voltage_A5 * 22.410 >= 11.01){
        Serial.print("Brightening LED on relay1 because A5 is ");
	      Serial.print(voltage_A5 * 22.410);
        Serial.println (" V");
        digitalWrite(RelayPin1, RELAY_ON);
        leds[NUM_LEDS] = CRGB(255,255,255); 
        FastLED.setBrightness( BRIGHTNESS_H ); //set full brightness
        FastLED.show();
        Serial.println("DRL Brightness level FULL");
        delay(500);
      } 
//      else {
//          //trun off relay1
//          Serial.print("Dimming LED off relay1 because A5 is ");
//          digitalWrite(RelayPin1, RELAY_OFF);
//         Serial.print(voltage_A5 * 22.410);
//          Serial.println (" V");   
//	        delay(500);
//    }
    if (voltage_A5 * 22.410 == 0)
      {
          //trun off relay1
          Serial.print("Dimming LED off relay1 because A5 is ");
          digitalWrite(RelayPin1, RELAY_OFF);
          Serial.print(voltage_A5 * 22.410);
          Serial.println (" V");   
	        delay(500);
    }
}