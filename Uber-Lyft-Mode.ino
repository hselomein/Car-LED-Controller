/*led controller uber lyft module
this will set the mode that the car led controller should run in

Store CurrMode in Flash/EEPROM
Button States:
Enum CurrMode
1 Normal Mode (Default)
2 Uber Mode (State 1)
3 Lyft Mode (State 2)

If Button is pressed increment CurrMode
 If CurrMode == Default
    Set Led Color to White
 If CurrMode == 1
    Set Led Color to Magenta
 If CurrMode == 2
    Set Led Color to Cyan
3th button press resets CurrValue to 1


How it works:
Normal Mode: (state 1)
read CurrMode
Set LED color based on Currmode
if CurrMode is null set leds to default (White)
if Currmode == 0, set CurrLED to White
if CurrMode == 1, Set CurrLED to Cyan
if CurrMode == 2, Set CurrLED to Magenta
Startup seqence sets currentled value
button pressed (State 2)
CurrMode get incremented to 2
CurrLED gets set to Cyan
leds turn to Cyan filled 
When done with uber mode:
button pressed again (State 3)
CurrMode get incremented to 3
CurrLED gets set to Mangenta
leds turn to Magenta filled
When done with Lyft Mode
button pressed again (State 1)
CurrMode get incremented to 1
CurrLED gets set to White
leds turn to White filled
Exit Function

*/
//-----------------------CODE--------------------------------------------

#include <Adafruit_NeoPixel.h>
#include <ezButton.h>

ezButton button(23);  // create ezButton object that attach to pin 23;
#define LED_PIN         13        // D13 => LED Controller Signal
#define NUM_LEDS        193  //193 leds is the length of the hood weather strip 163 + 15 leds on each side to show thru the headlights.

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

#define ANGRY_COLOR     leds.Color( 255, 60,  0,   0 )     //Amber
#define DEFAULT_COLOR   leds.Color( 0,  0,  0,   255 )     //White
#define LYFT_COLOR      leds.Color( 255,  0, 255,  0 )     //Magenta
#define UBER_COLOR      leds.Color( 0,  165, 255,  0 )     //Cyan

#define MAX_BRIGHTNESS  255
#define MIN_BRIGHTNESS  95
#define MED_BRIGHTNESS  127

#define RELAY_PIN_1     14        // D14 => In1 Relay
#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define DEBOUNCE_TIME  50 // the debounce time in millisecond, increase this time if it still chatters

int buttonState = 0;
int currColor;
int CurrMode = 0;

void setup(){
    int buttonState;
    button.setDebounceTime(DEBOUNCE_TIME); // set debounce time to 50 milliseconds    
    int currColor = DEFAULT_COLOR;
    Serial.begin(115200);
    digitalWrite(RELAY_PIN_1, RELAY_ON);    //Turn on relay to provide power for LEDs
    leds.begin();
    leds.show();
    leds.setBrightness(MIN_BRIGHTNESS);
}

void loop(){
  button.loop(); // MUST call the loop() function first
    int btnState = button.getState();
  Serial.println(btnState);
    if(button.isReleased()){  //button is pressed AND this is the first digitalRead() that the button is pressed
      CurrMode++;  
      if (CurrMode > 2) { //this number may need to change as well by 1
          CurrMode = 0;
      }
  }   
  UberLyftMode(CurrMode);
  leds.fill(currColor);
  leds.show();
}

void UberLyftMode(char CMode) {
    switch (CMode) {
      case 1:
          currColor = UBER_COLOR;
          Serial.println("LED color set to Uber Mode color (Cyan)");
          break;
      case 2:
          currColor = LYFT_COLOR;
          Serial.println("LED color set to Lyft Mode color (Magenta)");
          break;
      default:
          currColor = DEFAULT_COLOR;
          Serial.println("LED color set to Default Mode color (White)");
          break;
  }       
}