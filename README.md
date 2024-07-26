# Car-LED-Controller

Credits:
  - Corey Davis created the project and put together the prototypes.
  - Yves Avady wrote the code for this project.

This project controls an addressable LED Strip and LED matrix using the light inputs from a car electrical system.

With this project, we are taking an esp32 microcontroller and making an LED controller for the strip. 
The LED strip is a 12v WS2811 with 120 LEDs per meter


LED Strip Actions:
- Startup is a night rider style sweep 3 times until it is a solid white bar
- DRL Modes will control the LED strip brightness.

-  When DRLs are in Daytime Running Light Mode strip will be at full brightness
-  When DRLs are used as parking lights, the strip will be dimmed to a lower brightness
  
- Horn usage makes the lights turn dark amber to represent "Angry Mode"
- Indicators will show using a single chasing pattern that stays on for as long as the voltage is applied. This is to match the blink of the indicator circuit
- When hazard lights are on there will be a chasing pattern starting from the middle and ending at the ends
- There will be a button to cycle between Normal, Uber, and Lyft Modes
- Uber Mode, the led strip will be a Seafoam Greem color
- LYFT Mode, the led strip will be Magenta
There will be a physical switch installed to disable the light for law enforcement and/or vehicle inspection purposes

There are 5 voltage reading inputs 

  - DRL Left Sensing, 
  - DRL right Sensing,
  - Horn Sensing,
  - Left Indicator,
  - Right Indicator


At powerup LED strip will sweep 3 times each pass leaving the light on brighter until filled.
It'll stay filled white unless another mode is selected or an action happens

2 LED Matrices will be attached to the front and back of the vehicle and show 1 of 3 screens
  - Off or Blank for normal driving mode
  - Uber Mode: Shows Uber logo
  - Lyft Mode: Shows Lyft Logo

Actions:
DRL is in DRL Mode light will be at full brightness
DRL is in parking light mode and will be at half-brightness
When not in Uber or Lyft mode, the LED Matrix will be blank or off
Horn is pressed LED Strip will turn orange for the duration of the press
UBER Mode will turn the lights Cyan and the LED Matrix will show the Uber logo
Lyft Mide will turn the lights Magenta and the LED Matrix will show the Lyft Logo
Angry Mode will be some orangy red color, cant be RED as that is illegal in the front
The left indicator will show a chasing pattern, starting in the center and moving to the left side of the strip
The right indicator will show a chasing pattern, starting in the center and moving to the right side of the strip
Vehicle inspection mode, which will disable the lights, physical switch

There is a parts list so you can build this as well.

How to use with Platform-io IDE:
- You will need the Platformio-ide extension installed in VSCODE to compile the code
The src folder is where the code you want to compile and upload code to the ESP32
The main program code is in the file "main.cpp".

How to use with Arduino IDE
- The Arduino project files are located in the ArduinoIDE folder. This part is not complete still porting the code

This program is modular. You can enable or disable specific features by setting directives to true or false.

Build Configuration Options - This program is modular in the sense that you can have everything (LED Strip, LED Matrix) work or just cartain parts.  
See the Build Configuration Options to enable or disable certain features.

Examples:  
//Build Configuration Options  
  #define DEBUG false       //Enable serial output for debug, change to "false" to disable  
  #define SCREENTEST false  //To enable the boot up screen test, change to "true", to disable change to "false"  
  #define LED_MATRIX true   //Set to "true" if you want to use a 64x64 LED Matrix, "false" to disable  
  #define LED_STRIP true    //Set to "true" if you want to use the led strip, "false" to disable  
  #define NUM_MODES 2       //How many modes will the mode button handle (2 for Uber and Lyft signs)  

Code Notes:
- Car_LED_Controller-LED_Matrix_64x64.ino - this is the program for the 64x64 P2 LED Matrix for the Arduino ide, this still needs to be completed
- Part List - this is the shopping list of parts for this project


