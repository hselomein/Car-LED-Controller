/*led controller uber lyft module

Store CurrMode in Flash/EEPROM
Button States:
Enum CurrMode
1 Normal Mode (State 1)
2 Uber Mode (State 2)
3 Lyft Mode (State 3)

If Button is pressed increment CurrMode
 If CurrMode == 1
    Set Led Color to White
 If CurrMode == 2
    Set Led Color to Magenta
 If CurrMode == 3
    Set Led Color to Cyan
4th button press resets CurrValue to 1


How it works:
Normal Mode: (state 1)
read CurrMode
Set LED color based on Currmode
if CurrMode is null set leds to default (White)
if Currmode == 1, set CurrLED to White
if CurrMode == 2, Set CurrLED to Cyan
if CurrMode == 3, Set CurrLED to Magenta
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

#define BUTTON_PIN  10 // Button will need to be tied in with a reisitor to pull the leg LOW when the button is pressed



void setup()
    {
    pinMode(BUTTON_PIN, INPUT)
    Serial.begin(9600);
    }


void loop()
    {
        static int CurrMode = 0;
        buttonState = digitalRead(BUTTON_PIN);
        bool isPressed = false;

        if (buttonState == LOW && isPressed == false) //button is pressed AND this is the first digitalRead() that the button is pressed
            {
                isPressed = true;  //set to true, so this code will not run again until button released
                CurrMode++;  //not sure if this should be before or after the UberLyftMode function 
                UberLyftMode(); // a call to a separate function that performs the switch statement and subsequent evoked code

                
                if (CurrMode > 2) //this number may need to change as well by 1
                {
                    CurrMode = 0
                }
            } else if (digitalRead(btn) == HIGH)
            {
              isPressed = false; //button is released, variable reset
            }
        
    }

UberLyftMode() {
    switch (CurrMode) 
    {
        for (int CurrMode = 0; CurrMode < 3; CurrMode++)
            case 1:
                fill_solid(leds, NUM_LEDS, ANGRY_COLOR);
                Serial.println("LED color set to Uber Mode color (Cyan)");
                // statements
                break;
            case 2:
                fill_solid(leds, NUM_LEDS, ANGRY_COLOR);
                Serial.println("LED color set to Lyft Mode color (Magenta)");
                // statements
                break;
            default:
                fill_solid(leds, NUM_LEDS, DEFAULT_COLOR);
                Serial.println("LED color set to Default Mode color (White)");
                // statements
                break;
    }       
}
