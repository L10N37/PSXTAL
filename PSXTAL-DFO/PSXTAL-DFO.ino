#include <Adafruit_SI5351.h>
#define sensepin 2
Adafruit_SI5351 clockgen = Adafruit_SI5351();


void setup(){

DDRD = 0xFF;    // clear every port / pin and pull them all low as outputs temporarily  (The Port D stuff is especially important for videomode sensing to work)
PORTD = 0x00;   // any pins requiring different states are set up just after this
pinMode (sensepin, INPUT);
}


void loop(){


if (PIND == 0x4) {
//NTSC
  clockgen.setupPLL(SI5351_PLL_A, 30, 30, 450);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 13, 5, 5);
  while (PIND== 0x4); //lock up here and do nothing 
    }

if (PIND != 0x4) {
//PAL
  clockgen.setupPLLInt(SI5351_PLL_A, 16);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 7, 14, 27);
  while (PIND!= 0x4); //lock up here and do nothing 
    }


}

//not tested.
