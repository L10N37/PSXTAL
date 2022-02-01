/* PSXTAL V1.00B

  Adafruit SI5351 version, autoswitching region on this version, not controlled by button combos.
  RESET HAS BEING MOVED FROM DIGITAL PIN 2 TO DIGITAL PIN 9



  ===============================================================// CURRRENT BUTTON MAPS //===============================================================//


  SCPH110 DUAL SHOCK CONTROLLER IN DIGITAL MODE *********************************************************************************************************
  + SCPH1180 LARGE MADE IN JAPAN ANALOGUE CONTROLLER IN DIGITAL MODE
  + SCPH10010 PS2 DUAL SHOCK 2 ANALOGUE CONTROLLER IN DIGITAL MODE
  + SCPH110 PSOne DUAL SHOCK

  AUTO-DETECT                    Video Mode (PAL / NTSC)
  R1+TRIANGLE:   0xF3                  Short Reset (quick press, restart game on Xstation!)
  L1+TRIANGLE:   0xEB                        Long Reset (2second pulse, return to Xstation menu!)


  SCPH1080 DIGITAL ONLY PAD ******************************************************************************************************************************

  AUTO-DETECT                      Video Mode (PAL / NTSC)
  L2+DOWN:       0xDB                    Short Reset (quick press, restart game on Xstation!)
  R2+DOWN:       0xDD                         Long Reset (2second pulse, return to Xstation menu!)

  ===============================================================//====================== //===============================================================//



  SCPH1200 REGULAR COMMON ANALOGUE WITH RUMBLE************************************************************************************************************
  These controllers are often 'noisy',not sure if this is normal on all of these controllers. Changing SPI mode on another project fixed a similar issue.
  This is working fine so has been left alone


  AUTO-DETECT                      Video Mode (PAL / NTSC) (error corrected existing combo)
  L1+L2+R2:      0xC7                    Short Reset (quick press, restart game on Xstation!)
  L1+L2+R1+R2:   0x87                        Long Reset (2second pulse, return to Xstation menu!)
  //===============================================================//====================== //===============================================================//




                                             ,----,                ,--,
  ,-.----.                                  ,/   .`|             ,---.'|
  \    /  \    .--.--.   ,--,     ,--,    ,`   .'  : ,---,       |   | :
  |   :    \  /  /    '. |'. \   / .`|  ;    ;     /'  .' \      :   : |
  |   |  .\ :|  :  /`. / ; \ `\ /' / ;.'___,/    ,'/  ;    '.    |   ' :
  .   :  |: |;  |  |--`  `. \  /  / .'|    :     |:  :       \   ;   ; '
  |   |   \ :|  :  ;_     \  \/  / ./ ;    |.';  ;:  |   /\   \  '   | |__
  |   : .   / \  \    `.   \  \.'  /  `----'  |  ||  :  ' ;.   : |   | :.'|
  ;   | |`-'   `----.   \   \  ;  ;       '   :  ;|  |  ;/  \   \'   :    ;
  |   | ;      __ \  \  |  / \  \  \      |   |  ''  :  | \  \ ,'|   |  ./
  :   ' |     /  /`--'  / ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;
  :   : :    '--'.     /./__;  \  ;  \    ;   |.' |  :  :        |   ,/     NTSC / PAL wireless mode change (Automatic dual frequency selector)
  |   | :      `--'---' |   : / \  \  ;   '---'   |  | ,'        '---'      + wireless IN GAME RESET (This version is for PSX models using XTALS, not later models with a clock synth)
  `---'.|               ;   |/   \  ' |           `--''
  `---`               `---'     `--`
  VajskiDs Consoles 2021
  Adafruit SI5351 Version!

*/




#include <Adafruit_SI5351.h>
#include<SPI.h>
#define reset 9                           // NOTE: THIS RESET PIN HAS BEING CHANGED FROM 2 TO 9 ON THIS VERSION!
#define sensepin 2                        // PIN 2 NOW DETECTS REGION OF GAME FOR VIDEOMODE
volatile byte buffer = 0x00;
int counter = 0;
String NTSC_MODE = "LAUNCH";
Adafruit_SI5351 clockgen = Adafruit_SI5351();



void setup() {
  clockgen.begin();
  clockgen.enableOutputs(true);
  DDRD = 0xFF;    // clear every port / pin and pull them all low as outputs temporarily  (The Port D stuff is especially important for videomode sensing to work)
  PORTD = 0x00;   // any pins requiring different states are set up just after this,
  DDRB = 0xFF;    // leaves no floating inputs
  PORTB = 0x00;
  DDRC = 0xFF;
  PORTC = 0x00;
  pinMode (sensepin, INPUT);
  pinMode (reset, INPUT);
  //Serial.begin(74880);    // only for dev monitoring

   
}



void NTSC() {


  //25 * 30.06666 = 751.665 /14 = 53.69Mhz
  //Fractional PLL Setup (30.06666Mhz)
  clockgen.setupPLL(SI5351_PLL_A, 30, 30, 450);
  
  //Fractional Divider (14)
  clockgen.setupMultisynth(0, SI5351_PLL_A, 13, 5, 5);

  NTSC_MODE = "TRUE";

}


void PAL() {


  //25 * 16 = 400 /7.51 = 53.2Mhz
  //Integer PLL Setup (400Mhz)
  clockgen.setupPLLInt(SI5351_PLL_A, 16);

  //Fractional Divider (7.51)
  clockgen.setupMultisynth(0, SI5351_PLL_A, 7, 14, 27);

  NTSC_MODE = "FALSE";

}


void checkvideomode() {


  if (PIND == 0x4 && NTSC_MODE == "FALSE" || PIND == 0x4 && NTSC_MODE == "LAUNCH") { //digital read on sensepin causes video interference each read, using port status instead.

    NTSC();

  }


  if (PIND == 0x00 && NTSC_MODE == "TRUE" || PIND ==  0x00 && NTSC_MODE == "LAUNCH") {

    PAL();

  }

}




ISR (SPI_STC_vect) {            // interrupt routine function


  //capture IGR packets only
  if (SPDR == 0xF3 || SPDR == 0xDB || SPDR == 0xEB || SPDR == 0xDD || SPDR == 0x87 || SPDR == 0xC7) {
    buffer = SPDR;              // take a snap shot of the SPDR register and store it in 'buffer', but only if it's an error free combo capture byte
  }

  // capture correctable packets for long reset (SCPH1200)
  else  if (SPDR == 0xC3 || SPDR == 0xE1 || SPDR == 0xF0) {
    buffer = 0x87; //correct the packet
  }
  // capture correctable packets for short reset (SCPH1200)
  else if (SPDR == 0xE3 || SPDR == 0x8F) {
    buffer = 0xC7; // correct the packet
  }
  else {
    buffer = 0x00; // miscommunicated packet or no combo detected ! clear the buffer and continue monitoring.
  }
}




// buffer = SPDR;              dev/ debug, star out above code in interrupt function and uncomment
//}





void shortrst() {             // short reset

  buffer = 0x00;
  pinMode (reset, OUTPUT);
  digitalWrite (reset, 0);
  delay (40);
  pinMode (reset, INPUT);
}

void longrst () {            // long reset

  buffer = 0x00;
  pinMode (reset, OUTPUT);
  digitalWrite (reset, 0);
  delay (2000);
  pinMode (reset, INPUT);
}


void loop() {


  checkvideomode();



  if (counter == 10 && buffer ==  0xDB || counter == 10 && buffer ==  0xF3 || counter == 15 && buffer ==  0xC7) {        // short reset conditions
    shortrst();
  }



  if (counter == 10 && buffer ==  0xEB || counter == 10 && buffer ==  0xDD || counter == 15 && buffer ==  0x87) {         // long reset conditions
    longrst();
  }



  SPCR = 0xCB;                                                // SPI setup, 250khz, MSB (as slave), clock idle high, read on rising edge, SPI / SPI interrupts on,
  delay (500);
  //Serial.print("Combo Received from Controller: ");         // dev stuff
  //Serial.println(buffer, BIN);
  digitalWrite (SPE, 0);                                      // disable SPI




  if (buffer ==  0xF3 || buffer ==  0xDB || buffer ==  0xEB || buffer ==  0xDD || buffer ==  0x87 || buffer ==  0xC7) { // ensure the combo has to be held for at least ~5s
    counter++;
    loop();
  }
  else {                                                      // if key combo is not being pressed, reset button combos 'pressed time' counter
    counter = 0;
    loop();
  }
}
