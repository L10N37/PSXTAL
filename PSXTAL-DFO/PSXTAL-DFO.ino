#include <Wire.h>
#include <Adafruit_SI5351.h>
#define sensepin 2
Adafruit_SI5351 clockgen = Adafruit_SI5351();
bool NTSC = false;

void setup(){

Wire.begin();             // initialize I2C interface
Wire.setClock(400000);    // set I2C clock speed to 400 kHz
clockgen.begin();
clockgen.enableOutputs(true);
DDRD = 0xFF;    // Set known port state //
PORTD = 0x00;   //                      //
DDRB = 0xFF;  
PORTB = 0x00;
DDRC = 0xFF;
PORTC = 0x00;
pinMode (sensepin, INPUT);
}


void loop(){


if (PIND == 0x4 && NTSC == false) {
//NTSC
  clockgen.setupPLL(SI5351_PLL_A, 30, 30, 450);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 13, 5, 5);
  NTSC = true;
    }

if (PIND == 0x00 && NTSC == true) {
//PAL
  clockgen.setupPLLInt(SI5351_PLL_A, 16);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 7, 14, 27);
  NTSC = false;
    }

}

/*
Use 8Mhz internal osc

LFUSE = 0xC2
HFUSE = 0xF9
EFUSE = 0xFF
*/
