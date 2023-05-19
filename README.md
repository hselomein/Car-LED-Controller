# Car-LED-Controller

Credits:
  - Corey Davis created the project and put together the prototypes.
  - Yves Avady wrote the code for this project.

This project controls an addressible LED Strip using the inputs from a car electical system.

With this project we are taking and esp32 arduino and making a LED controller for the strip. 
The LED stip is a 5v sk6812 with 144 LEDs per meter

LED Strip Actions:
Startup is a night rider style sweep 3 times untill its a solid white bar
DRL Modes will control the LED strip bightness.

  When DRLs are in Daytime Running Light Mode strip will be full brightness
  WHen DRLs are used a parking lights, then the strip will be dimmed to a lower brightness
  
Horn usage makes the lights turn dark amber to represent "Angry Mode"
There will be a botton to cycle between Normal, Uber and Lyft Modes
Uber Mode, the led strip will be CYAN
LYFT Mode, the ;et strip will be Magenta
There will be a physcal switch installed to disable light for law enformcent / vehicle inpsection purposes

There will be 4 voltage reading inputs 2 will be reserved for future uses

  - DRL Sensing,
  - Horn Sensing,
  - Reserved,
  - Reserved

The number of LEDs in strip 144 pixels per meter, x4 individual pixelets, SK6812 Chip 

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
Vehicle inpection mode, that will disable the lights, physical switch

There is a parts list so you can build this as well.

How to use with Platformio IDE:
- You will need the Platformio-ide extension installed in VSCODE in order to compile the code
The src folder is where the code you want to compile and upload code to the ESP32
Main program code is in the file main.cpp

How to use with Arduino IDE
The Arduino project files are located in the ArduinoIDE folder.


Code Notes:
- led_controller-ESP32.ino  - this is the program for the esp32 architecture
- Car_LED_Controller-LED_Matrix_64x64.ino - this is the program for the 64x64 P2 LED Matrix
- Part List - this is the shopping list of parts for this project
- PXMatrixTest.ino - Test program for 64x64 P2 LED Matrix using the PXMatrix library

