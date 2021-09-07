True NTSC 60hz / PAL 50hz selector and In Game Reset, all controlled by button combos on the controller! for PS1!

Supports long reset (back to Xstation Menu) / Short reset (back to game being played on Xstation)
Supports a small PCB i made that routes power to either system regions XTAL.

PSXTAL is pretty well finished, all the genuine controller types are supported anyway. I can't say whether aftermarket ones will work as there's too many, and I'll never bother getting them.

You'd only have to monitor through the serial port (just comment out everything bar the snapshot in the interrupt so you see streaming '11111') then make sure for a 3 button combo, you've actually flipped 3 bits. Add this hex value to the according 'OR's' in the appropriate sections and you've added your controller support!

VajskiDs Consoles 2021


