// Based off an EXTREMELY modified version of GolamMostafas code @ [https://forum.arduino.cc/t/spi-slave-mode-example-code/66617/4]
// This was the giant kick starter to get things going, it intercepted and read inputs straight away! Google really done it's job, Cheers Golam!
// Arduino wires currently soldered direct to the rear of the controller / memory card port (clock = D13, data = D11, attention = D10)
// 5v to arduino (nothings ever taken 'high' bar the XTALs) with 220uf bypass cap.
// Made for my dual Xtal PCB's, arduino routes power to either crystal, switching video modes wirelessly.



//===============================================================// CURRRENT BUTTON MAPS //===============================================================//
// Video mode combos need to be held approximately 5  seconds MINIMUM - packet errors can extend this
// Reset mode combos are VERY well guarded for accidental flags, may need to be held for 10 to 15 seconds!
// you can lower the counters from 14 for a quicker acceptance of this combo.



// SCPH110 DUAL SHOCK CONTROLLER IN DIGITAL MODE *********************************************************************************************************
// + SCPH1180 LARGE MADE IN JAPAN ANALOGUE CONTROLLER IN DIGITAL MODE
// + SCPH10010 PS2 DUAL SHOCK 2 ANALOGUE CONTROLLER IN DIGITAL MODE


// L1+R1+CIRCLE:  0xE5         Video Mode (PAL / NTSC)
// R1+TRIANGLE:   0XF3                  Short Reset (quick press, restart game on Xstation!)
// L1+TRIANGLE:   0xEB                        Long Reset (2second pulse, return to Xstation menu!)



// SCPH1080 DIGITAL ONLY PAD ******************************************************************************************************************************

// L2+R2+DOWN:    0xD9          Video Mode (PAL / NTSC)
// L2+DOWN:       0xDB                    Short Reset (quick press, restart game on Xstation!)
// R2+DOWN:       0xDD                         Long Reset (2second pulse, return to Xstation menu!)

//===============================================================//====================== //===============================================================//

                                            ,----,                ,--,    
//  ,-.----.                                  ,/   .`|             ,---.'|    
//  \    /  \    .--.--.   ,--,     ,--,    ,`   .'  : ,---,       |   | :    
//  |   :    \  /  /    '. |'. \   / .`|  ;    ;     /'  .' \      :   : |    
//  |   |  .\ :|  :  /`. / ; \ `\ /' / ;.'___,/    ,'/  ;    '.    |   ' :    
//  .   :  |: |;  |  |--`  `. \  /  / .'|    :     |:  :       \   ;   ; '    
//  |   |   \ :|  :  ;_     \  \/  / ./ ;    |.';  ;:  |   /\   \  '   | |__  
//  |   : .   / \  \    `.   \  \.'  /  `----'  |  ||  :  ' ;.   : |   | :.'| 
//  ;   | |`-'   `----.   \   \  ;  ;       '   :  ;|  |  ;/  \   \'   :    ; 
//  |   | ;      __ \  \  |  / \  \  \      |   |  ''  :  | \  \ ,'|   |  ./  
//  :   ' |     /  /`--'  / ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;    
//  :   : :    '--'.     /./__;  \  ;  \    ;   |.' |  :  :        |   ,/     NTSC / PAL wireless mode change (dual XTAL selector)
//  |   | :      `--'---' |   : / \  \  ;   '---'   |  | ,'        '---'      + wireless IN GAME RESET
//  `---'.|               ;   |/   \  ' |           `--''                     
//  `---`               `---'     `--`                                      
                                                            // VajskiDs Consoles 2021                                                                         
                                                            
#include<SPI.h>
#define NTSC 2
#define PAL 3
#define reset 4
static int lastmode = 0; //not used
volatile byte buffer = 0x00;
int NTSC_ = 1; // flags signifying output mode, instead of digital reading the pins just check the flag
int PAL_ = 0;  // to determine the output mode instead
int counter = 0;


// TO DO , work out how to store last video mode in flash / progmem, and keep the state for next boot, add regular dual shock (if it differs?)


void setup() {

  DDRD = 0x00;    // clear every port / pin and pull them all low as outputs temporarily
  PORTD = 0x00;   // any pins requiring different states are set up just after this,
  DDRB = 0x00;    // leaves no floating inputs
  PORTB = 0x00;
  DDRC = 0x00;
  PORTC = 0x00;

  //default pin state (Crystals VCC to  D2 defaults on)
  pinMode (NTSC, OUTPUT);
  digitalWrite (NTSC, 1);   // video 1 active @ boot (you can wire this the other way around or change the source code)
  pinMode (PAL, OUTPUT);
  digitalWrite (PAL, 0);
  pinMode (reset, INPUT);
 // Serial.begin(74880);  // only for dev monitoring
  SPI.begin();
}


ISR (SPI_STC_vect) {            // interrupt routine function

  if (SPDR == 0xD9 || SPDR == 0xE5 || SPDR == 0xF3 || SPDR == 0xDB || SPDR == 0xEB || SPDR == 0xDD )

    buffer = SPDR;              // take a snap shot of the SPDR register and store it in 'buffer', but only if it's an error free combo capture byte

  else {

    buffer = 0x00;              // miscommunicated packet!clear the buffer and try again
  }
}

void sw_video() {


  if (PAL_ == 1) {              // 60HZ NTSC

    digitalWrite (NTSC, 1);
    digitalWrite (PAL, 0);
    NTSC_ = 1;
    PAL_ = 0;
    buffer == 0x00;


  }
  else if (NTSC_ == 1)  {       // 50HZ PAL

    digitalWrite (NTSC, 0);
    digitalWrite (PAL, 1);
    NTSC_ = 0;
    PAL_ = 1;
    buffer = 0x00;

  }
}


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


  if (counter == 8 && buffer == 0xD9 || counter == 8 && buffer == 0xE5) {             // this condition is true once video mode switch combo has being held for ~5s
    sw_video();                                                                       // so we can jump here and switch modes
  }


  if (counter == 14 && buffer ==  0xDB || counter == 14 && buffer ==  0xF3) {         // short reset conditions
    shortrst();
  }



  if (counter == 14 && buffer ==  0xEB || counter == 14 && buffer ==  0xDD) {         // long reset conditions
    longrst();
  }



  SPCR = 0xCB;                                  // SPI setup, 250khz, MSB (as slave), clock idle high, read on rising edge, SPI / SPI interrupts on,
  //Serial.print("Combo Received from Controller: ");   // dev stuff
  //Serial.println(buffer, BIN);
  digitalWrite (SPE, 0);                        // disable SPI
  delay (500);


  if (buffer == 0xE5 || buffer == 0xD9 || buffer ==  0xF3 || buffer ==  0xDB || buffer ==  0xEB || buffer ==  0xDD) { // ensure the combo has to be held for at least ~5s
    counter++;
    loop();
  }
  else {                                        // if key combo is not being pressed, reset button combos 'pressed time' counter
    counter = 0;
    loop();
  }
}
