# Car-LED-Controller
This project controls an addressible LED Strip using the inputs from a car electical system.

Power: 0v - 15v 
What it powers, Arduino, 3x 5v buck converters, 1 white 60" led strip  
Input1: 1 voltage divier circuit that divides automotive level voltage 11 times: 1-1Mohm and 1-100kohm resistors
Expected input ranges: 0v to 15v
Expected divided voltage: 0v ,12v = 1.091v ,15v = 1.364v
Input2: 1 voltage divier circuit that divides automotive level voltage 11 times: 1-1Mohm and 1-100kohm resistors
Expected input ranges: 0v to 15v
Expected divided voltage: 0v ,12v = 1.091v ,15v = 1.364v

voltage divider
DRL -> 1M -> sense pin -> 47k -> ground

pseudo code
Buf = 1 //+-1v
Div = 50 / 1000 //divider value
LBL = 2 //2v low beam
DRL = 12 //12v DRL 
Brightness = in / Div
If abs(brightness - LBL) < Buf then lowBeam=true
Also, to set led brightness
LEDv = brightness * 256 / 12
If LEDv > 256 then LEDv =256
If LEDv < 0 then LEDv =0c ch


Arduino setup
Pins to device mapping
D2 => In1 Relay
D3 => In2 Relay
A2 => DRL Sense
A3 => PK_L Sense
A4 => Hn Sense
A5 => HiBm Sense
number of LEDs in strip 30 pixels, 90 individual pixels, WS2811 controller 

At powerup LED strip will sweep 3 times each pass leaving the light on brighter untill filled.
It'll stay filled white unless another mode is selected or an action happens

Actions:
DRL are in DRL Mode light will be full birghtness
DRL is in parking light mode wiull be half brightness
Horn is pressed LED Strip will trun orange for the duration of the press
UBER Mode will turn the lights Cyan
Lyft Mide will trun the lights Magenta
Angry Mode will be some orangy red color, cant be RED as that is illegal in the front
Vehicle inpection mode, that will disable the lights


