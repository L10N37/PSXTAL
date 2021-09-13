// PSXTAL V1.00
// Based off an EXTREMELY modified version of GolamMostafas code @ [https://forum.arduino.cc/t/spi-slave-mode-example-code/66617/4]
// This was the giant kick starter to get things going, it intercepted and read inputs straight away! Google really done it's job, Cheers Golam!
// Arduino wires currently soldered direct to the rear of the controller / memory card port (clock = D13, data = D11, attention = D10)
// 5v to arduino (nothings ever taken 'high' bar the XTALs) with 220uf bypass cap.
// Made for my dual Xtal PCB's, arduino routes power to either crystal, switching video modes wirelessly.



//===============================================================// CURRRENT BUTTON MAPS //===============================================================//
// Video mode combos need to be held approximately 5  seconds MINIMUM - packet errors can extend this (regular PS1 analogue has error correction)
// Reset mode combos are VERY well guarded for accidental flags, may need to be held for 10 to 15 seconds! (regular PS1 analogue has error correction)
// you can lower the counters from 10 for a quicker acceptance of this combo.



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



// SCPH1200 REGULAR COMMON ANALOGUE WITH RUMBLE************************************************************************************************************
// regrettably sold a collection of mint ones, the one I have left is sometimes really 'noisy',not sure if this
// is normal on all of these controllers.
// At times it's absolutely perfect with each combo and matches the others above, but sometimes it get's in a mood, and the short reset combo starts throwing an
// E5h packet, and when it's sequentially (videomode switch condition coming sooner than a reset doesn't help) it will switch video modes instead of resetting.

// ...SO remapped for peace of mind , and error corrected for this controller so it won't really ever drop the loop


// L1+R1+CIRCLE:  0xE5          Video Mode (PAL / NTSC) (error corrected existing combo)
// L1+L2+R2:      0xC7                    Short Reset (quick press, restart game on Xstation!)
// L1+L2+R1+R2:   0x87                        Long Reset (2second pulse, return to Xstation menu!)
//===============================================================//====================== //===============================================================//




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
//  :   ' |     /  /`--'  / ;  /\  \  \     '   :  ||  |  '  '--'  ;   : ;
//  :   : :    '--'.     /./__;  \  ;  \    ;   |.' |  :  :        |   ,/     NTSC / PAL wireless mode change (dual XTAL selector)
//  |   | :      `--'---' |   : / \  \  ;   '---'   |  | ,'        '---'      + wireless IN GAME RESET
//  `---'.|               ;   |/   \  ' |           `--''
//  `---`               `---'     `--`
// VajskiDs Consoles 2021


#include<SPI.h>
#include <EEPROM.h>
#define NTSC 2
#define PAL 3
#define reset 4
static int lastmode = 0; //not used
volatile byte buffer = 0x00;
int NTSC_ = 0; // flags signifying output mode, instead of digital reading the pins just check the flag
int PAL_ = 0;  // to determine the output mode instead
int counter = 0;



void setup() {

  DDRD = 0xFF;    // clear every port / pin and pull them all low as outputs temporarily
  PORTD = 0x00;   // any pins requiring different states are set up just after this,
  DDRB = 0xFF;    // leaves no floating inputs
  PORTB = 0x00;
  DDRC = 0xFF;
  PORTC = 0x00;

  //default pin state (Crystals VCC to  D2 defaults on)
  pinMode (NTSC, OUTPUT);
  pinMode (PAL, OUTPUT);
  pinMode (reset, INPUT);
  //Serial.begin(74880);    // only for dev monitoring



  //***************************************************************************************************************************************************************
  if (EEPROM.read(0) == 0x4E || EEPROM.read(0) == 0x50) {     // Check EEPROM for a valid video mode, it won't exist at first ever boot
    goto notfirstboot;                                        // If it exists, jump to 'notfirstboot', don't waste write cycles on the EEPROMs finite life (~100,000 writes)
  }
  else {
    EEPROM.write(0, 0x50);                                    // First ever use, so write a valid video mode (NTSC)
  }
notfirstboot:
  //***************************************************************************************************************************************************************



  // Retrieve last video mode from non volatile EEPROM mem and set initial video mode
  if (EEPROM.read(0) == 0x4E) {
    digitalWrite (NTSC, 0);
    digitalWrite (PAL, 1);
    PAL_ = 1;
  }
  else if (EEPROM.read(0) == 0x50) {
    digitalWrite (PAL, 0);
    digitalWrite (NTSC, 1);
    NTSC_ = 1;
  }

}




ISR (SPI_STC_vect) {            // interrupt routine function




  //capture correct packets for all functions
  if (SPDR == 0xE5 || SPDR == 0xF3 || SPDR == 0xEB || SPDR == 0xD9 || SPDR == 0xDB || SPDR == 0xDD || SPDR == 0xE5 || SPDR == 0x87 || SPDR == 0xC7) {
    buffer = SPDR;              // take a snap shot of the SPDR register and store it in 'buffer', but only if it's an error free combo capture byte
  }

  // capture correctable packets for video change (SCPH1200)
  else if (SPDR == 0xF9 || SPDR == 0xF2) {
    buffer = 0xE5; //correct the packet
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
    buffer = 0x00; // miscommunicated packet or no combo detected ! clear the buffer and try again
  }
}




// buffer = SPDR;              dev/ debug, star out above code in interrupt function and uncomment
//}



void sw_video() {


  if (PAL_ == 1) {              // 60HZ NTSC

    digitalWrite (NTSC, 1);
    digitalWrite (PAL, 0);
    NTSC_ = 1;
    PAL_ = 0;
    buffer = 0x00;
    EEPROM.update(0, 0x50);   //'P'


  }
  else if (NTSC_ == 1)  {       // 50HZ PAL

    digitalWrite (NTSC, 0);
    digitalWrite (PAL, 1);
    NTSC_ = 0;
    PAL_ = 1;
    buffer = 0x00;
    EEPROM.update(0, 0x4E);   //'N'

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


  if (counter == 10 && buffer ==  0xDB || counter == 10 && buffer ==  0xF3 || counter == 15 && buffer ==  0xC7) {        // short reset conditions
    shortrst();
  }



  if (counter == 10 && buffer ==  0xEB || counter == 10 && buffer ==  0xDD || counter == 15 && buffer ==  0x87) {         // long reset conditions
    longrst();
  }



  SPCR = 0xCB;                                                // SPI setup, 250khz, MSB (as slave), clock idle high, read on rising edge, SPI / SPI interrupts on,
  delay (500);
  //Serial.print("Combo Received from Controller: ");           // dev stuff
  //Serial.println(buffer, BIN);
  digitalWrite (SPE, 0);                                      // disable SPI


  if (buffer == 0xE5 || buffer == 0xD9 || buffer ==  0xF3 || buffer ==  0xDB || buffer ==  0xEB || buffer ==  0xDD || buffer ==  0x87 || buffer ==  0xC7) { // ensure the combo has to be held for at least ~5s
    counter++;
    loop();
  }
  else {                                                      // if key combo is not being pressed, reset button combos 'pressed time' counter
    counter = 0;
    loop();
  }
}




/*
  Analogue controller error correction example:

  Combo Received from Controller: 11000111 C7h       0xC7 is the correct packet based off % received during combo  / the other packets are breaking the loop
  Combo Received from Controller: 11100011 E3h
  Combo Received from Controller: 11100011 E3h
  Combo Received from Controller: 10001111 8Fh
  Combo Received from Controller: 10001111 8Fh
  Combo Received from Controller: 10001111 8Fh
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 10001111 8Fh
  Combo Received from Controller: 11100011 E3h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 10001111 8Fh
  Combo Received from Controller: 11000111 C7h
  Combo Received from Controller: 11000111 C7h
*/
