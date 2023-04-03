# Car-LED-Controller
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

Code Notes:
- led_controller-ESP32.ino  - this is the program for the esp32 architecture
- led_controller-AVR.ino    - this is the unfinished program for the AVR architecture
- Part List - this is the shopping list of parts for this project

Corey Davis created the project and put together the prototypes.
Yves Avady wrote the code for this project.



