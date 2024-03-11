#include <Wire.h>
#include <Adafruit_SI5351.h>
#define senseHigh bitRead(PIND, 2) == HIGH
Adafruit_SI5351 clockgen = Adafruit_SI5351();
bool NTSC = false;

void setup(){

Wire.begin();             // initialize I2C interface
Wire.setClock(400000);    // set I2C clock speed to 400 kHz
clockgen.begin();
clockgen.enableOutputs(true);
bitClear (DDRD, 2);
}

void loop(){

if (senseHigh && !NTSC) {
//NTSC

	/*GPU*/
  clockgen.setupPLL(SI5351_PLL_A, 30, 30, 450);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 13, 5, 5);
	/*SubC*/
  clockgen.setupPLLInt(SI5351_PLL_B, 25);  	// 25 * 25 = 625
  clockgen.setupMultisynth(1, SI5351_PLL_B, 174,200, 331);
  NTSC = true;
    }

if (!senseHigh && NTSC) {
//PAL

	/*GPU*/
  clockgen.setupPLLInt(SI5351_PLL_A, 16);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 7, 14, 27);
	/*SubC*/
  clockgen.setupPLLInt(SI5351_PLL_B, 36);
  clockgen.setupMultisynth(1, SI5351_PLL_B, 202, 194346 , 195429);
  NTSC = false;
    }

}

/*
Use 8Mhz internal osc

LFUSE = 0xC2
HFUSE = 0xF9
EFUSE = 0xFF
*/
