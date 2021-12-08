
//                                              ,----,                ,--,
//  ,-.----.                                  ,/   .`|             ,---.'|
//  \    /  \    .--.--.   ,--,     ,--,    ,`   .'  : ,---,       |   | :
//  |   :    \  /  /    '. |'. \   / .`|  ;    ;     /'  .' \      :   : |
//  |   |  .\ :|  :  /`. / ; \ `\ /' / ;.'___,/    ,'/  ;    '.    |   ' :
//  .   :  |: |;  |  |--`  `. \  /  / .'|    :     |:  :       \   ;   ; '
//  |   |   \ :|  :  ;_     \  \/  / ./ ;    |.';  ;:  |   /\   \  '   | |__
//  |   : .   / \  \    `.   \  \.'  /  `----'  |  ||  :  ' ;.   : |   | :.'|
//  ;   | |`-'   `----.   \   \  ;  ;       '   :  ;|  |  ;/  \   \'   :    ;
//  |   | ;      __ \  \  |  / \  \  \      |   |  ''  :  | \  \ ,'|   |  ./
//  :   ' |     /  /`--'  / ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;    PU22+ version!
//  :   : :    '--'.     /./__;  \  ;  \    ;   |.' |  :  :        |   ,/     True NTSC / PAL modes, AUTO-SWITCHING
//  |   | :      `--'---' |   : / \  \  ;   '---'   |  | ,'        '---'
//  `---'.|               ;   |/   \  ' |           `--''
//  `---`               `---'     `--`
// VajskiDs Consoles 2021
// For Adafruit SI5351 clock gen boards


#include <Adafruit_SI5351.h>
#define sensepin 2
Adafruit_SI5351 clockgen = Adafruit_SI5351();
void NTSC();
void PAL();
String NTSC_MODE = "LAUNCH";


void setup() {
  DDRD =  0xFF;  //Set PORT D to a known state! All outputs- ALL low - Required!
  PORTD = 0x00;  //*************************************************************
  pinMode (sensepin, INPUT);
  clockgen.begin();
  clockgen.enableOutputs(true);


  //PLL_A:  751.6665
  clockgen.setupPLL(SI5351_PLL_A, 30, 30, 450);
  //PLL_B:  400
  clockgen.setupPLLInt(SI5351_PLL_B, 16);


  //Fractional Div: 11.08
  clockgen.setupMultisynth(2, SI5351_PLL_A, 11, 2, 25);    //CLK2 CPU
  //67.8401Mhz


  //PAL console starts high on sense pin/NTSC for about 6 seconds, screen will show 480/60i , then goes low and launches in PAL 575/50i -delay skips these switches
  //console should launch straight straight into it's native video mode.

  delay (6000);

}

void loop() {


  if (PIND == 0x4 && NTSC_MODE == "FALSE" || PIND == 0x4 && NTSC_MODE == "LAUNCH") { //digital read on sensepin causes video interference each read, using port status instead.

    NTSC();

  }


  if (PIND == 0x00 && NTSC_MODE == "TRUE" || PIND ==  0x00 && NTSC_MODE == "LAUNCH") {

    PAL();

  }

}


void NTSC() {

  //==============NTSC GPU CLK==============
  //25 * 30.06666 = 751.665 /14 = 53.69Mhz
  clockgen.setupMultisynth(0, SI5351_PLL_A, 13, 5, 5);  //CLK0 GPU


  //==============NTSC SUB-CARRIER CLK==============
  //25 * 30.06666 = 751.665 /210 = 3.58Mhz
  //clockgen.setupMultisynth(1, SI5351_PLL_A, 209, 5, 5); //CLK1 SUB not a clean clock, works though
  clockgen.setupMultisynth(1, SI5351_PLL_B, 111, 8, 11);  //PLL_B derived from integer multiplication, used on PAL Sub.C as well, CLEAN
  NTSC_MODE = "TRUE";

}


void PAL() {

  //==============PAL GPU CLK===============
  clockgen.setupMultisynth(0, SI5351_PLL_A, 14, 2, 16); //CLK0 GPU

  //PLL_A:  751.6665
  //PLL_B:  400

  //==============PAL SUB-CARRIER CLK=============== (ORIGINAL FREQ = 4.43358Mhz)
  //clockgen.setupMultisynth(1, SI5351_PLL_A, 169, 2, 3);   //CLK1 SUB (= 4.43027 but B&W)
  //clockgen.setupMultisynth(1, SI5351_PLL_A, 169, 9, 31);  // = 4.44013   (rainbow effect)
  clockgen.setupMultisynth(1, SI5351_PLL_B, 90, 3, 13);     // = 4.43310   (nailed it)

  NTSC_MODE = "FALSE";

}
