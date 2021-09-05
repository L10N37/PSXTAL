# PSXTAL
Just learning arduino, this took a while for me to be happy with it. Complete noob here.
The code simply looks for a keypress combination and routes VCC to either Digital Pin 6 or 7.
The idea is to switch XTALS in a console to swap between NTSC/PAL video modes instead of using a manual switch.
The XTALS should only consume power in the low nano amps so this should be ok.
No idea how I'm going to implement it in the console, it will likely need heavy modification.




Update:
PSXTAL is pretty well finished, may need to add support for another controller, it might map differently.
Also contains in game reset for short or long pulses (awesome for Xstation)
 - VajskiDs
