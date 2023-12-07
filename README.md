# Car-LED-Controller

Credits:
  - Corey Davis created the project and put together the prototypes.
  - Yves Avady wrote the code for this project.

This project controls an addressible LED Strip and LED matrix using the light inputs from a car electical system.

With this project we are taking an esp32 arduino and making a LED controller for the strip. 
The LED stip is a 12v WS2811 with 120 LEDs per meter
The number of LEDs in strip 144 pixels per meter, x4 individual pixelets, WS2811 Chip 

LED Strip Actions:
Startup is a night rider style sweep 3 times untill its a solid white bar
DRL Modes will control the LED strip bightness.

  When DRLs are in Daytime Running Light Mode strip will be full brightness
  WHen DRLs are used a parking lights, then the strip will be dimmed to a lower brightness
  
Horn usage makes the lights turn dark amber to represent "Angry Mode"
Indicators will show using a single chasing pattern that stays on for as long as voltage is applied. THis is to match the blink of the indicator circuit
When hazard lights are on there will be a chasing pattern starting from the middle and ending at the ends
There will be a botton to cycle between Normal, Uber and Lyft Modes
Uber Mode, the led strip will be CYAN
LYFT Mode, the ;et strip will be Magenta
There will be a physcal switch installed to disable light for law enformcent / vehicle inpsection purposes

There will be 5 voltage reading inputs 2 will be reserved for future uses

  - DRL Left Sensing,
  - DRL right Sensing,
  - Horn Sensing,
  - Left Indicator,
  - Right Indicator


At powerup LED strip will sweep 3 times each pass leaving the light on brighter untill filled.
It'll stay filled white unless another mode is selected or an action happens

2 LED Matrices will be attached the front and back of the vehicle and show 1 of 3 screens
  - Off or Blank for normal driving mode
  - Uber Mode: Shows Uber logo
  - Lyft Mode: Shows Lyft Logo

Actions:
DRL are in DRL Mode light will be full brightness
DRL is in parking light mode will be half brightness
WHen not in Uber of Lyft mode, the LED Matrix will be blank or off
Horn is pressed LED Strip will turn orange for the duration of the press
UBER Mode will turn the lights Cyan and the LED Matrix will show the Uber logo
Lyft Mide will turn the lights Magenta and the LED Matrix will show the Lyft Logo
Angry Mode will be some orangy red color, cant be RED as that is illegal in the front
Left indicator on will show a chasing patterm, starting in the center and moving to the left side of the strip
Right indicator on will show a chasing pattern, starting in the center and moving to the right side of the strip
Vehicle inpection mode, that will disable the lights, physical switch

There is a parts list so you can build this as well.

How to use with Platformio IDE:
- You will need the Platformio-ide extension installed in VSCODE in order to compile the code
The src folder is where the code you want to compile and upload code to the ESP32
Main program code is in the file main.cpp

How to use with Arduino IDE
The Arduino project files are located in the ArduinoIDE folder. This part is not complete still porting the code

This program is modular. Uou can enable or disable specific features by setting directives to true of false.

Examples:
//Build Configuration Options
  #define DEBUG false       //Enable serial output for debug, change to "false" to disable
  #define SCREENTEST false  //To enable the boot up screen test, change to, to disable change to "false"
  #define LED_MATRIX true   //Set to false if you want to use a 64x64 LED Matrix
//#define LED_STRIP         //for future development
  #define NUM_MODES 2       //How many modes will the mode button handle (2 for Uber and Lyft signs)
  #define LEFT_IND false    //enable left indicator code for testing
  #define RIGHT_IND false   //enable right indicator code for testing
  #define V9_PCB true       //enble if you are using V9 LED Controller PCB

Code Notes:
- Car_LED_Controller-LED_Matrix_64x64.ino - this is the program for the 64x64 P2 LED Matrix for the arduino ide
- Part List - this is the shopping list of parts for this project


