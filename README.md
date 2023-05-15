﻿# Car-LED-Controller

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

Actions:
DRL are in DRL Mode light will be full brightness
DRL is in parking light mode will be half brightness
Horn is pressed LED Strip will trun orange for the duration of the press
UBER Mode will turn the lights Cyan
Lyft Mide will trun the lights Magenta
Angry Mode will be some orangy red color, cant be RED as that is illegal in the front
Vehicle inpection mode, that will disable the lights, physical switch

There is a parts list so you can build this as well.

How to use with Platformio IDE:
- You will need the Platformio-ide extension installed in VSCODE in order to compile the code
The src folder is where the code you want to compile and upload code to the ESP32
Edit code in the files outside the src folder
Copy the file to the src folder and rename file file extension to .cpp
Build as normal


Code Notes:
- led_controller-ESP32.ino  - this is the program for the esp32 architecture
- Car_LED_Controller-LED_Matrix_64x64.ino - this is the program for the 64x64 P2 LED Matrix
- Part List - this is the shopping list of parts for this project
- PXMatrixTest.ino - Test program for 64x64 P2 LED Matrix using the PXMatrix library

Corey Davis created the project and put together the prototypes.
Yves Avady wrote the code for this project.



