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