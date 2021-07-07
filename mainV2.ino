//                                          L10N37 / VAJSKIDS CONSOLES 50/60HZ PSXTAL SELECTOR , June 2021
//                                                                                ...Techicially version 31,204
// wasn't even thinking about this code and it just came to mind that there was an if, else if, else in the main.. realised it could just be swapped 
// to if (ntsc) else (pal)
// as oppose to if (no keypress / then PAL, else if  keypress / then NTSC / else PAL) but then I decided to do a restructure, it literally took 2 minutes 
// and it's much better now
// Now we hold the key combo for 2 seconds, then release and there's a smooth transition back and forth. Prior if you held the combo it would eventually switch 
// even whilst still  holding the key combo and I also couldn't work out why every now and then it would jump from NTSC to PAL sooner than the delay function 
// should have permitted (but never the other way around). Seems to be fixed.


/*Default boot mode is PAL, boot into NTSC  straight up with L1, R2, START or select held (or any combination of) <--This was changed
  switch between video modes with L1+R2+START+SELECT held simultaneously for 7 seconds */

const int NTSC_MODE     = (0B01000000);   //  ***   set D register to two modes for easier readability ***
const int PAL_MODE      = (0B10000000);   //   ***   BIT 7 OR BIT 8 high dependant on mode              ***
const int keypress_none  = (0B001111);    // set value of register (B) for default (no keys pressed @ boot)
const int keypress       = (0B000000);    // L1+R1+START+SELECT


void setup() {
  //Define registers

#define pin_register PINB  // 
#define MODE_LATCH PORTD   // Port D contains one of 2 states, latch onto PAL or latch onto NTSC mode
#define NEW_MODE PORTB     // for PORT B soft writes to switch video modes smoothly
  //Define pins

#define PAL_pin 7         // NTSC XTAL VCC   PORT D
#define NTSC_pin 6        // PAL XTAL VCC     PORT D
#define ST_press 11       //PORT B
#define L1_press 10       //PORT B
#define R1_press 9        //PORT B
#define SEL_press 8       //PORT B



  //Set pin modes

  pinMode (12, INPUT);          //13   Unused pin(part of port B digital pins 13:8 - little endian)
  pinMode (13, INPUT);          //12   Unused pin(part of port B digital pins 13:8 - little endian)
  pinMode (ST_press, INPUT_PULLUP);    //11                // input for START keypress
  pinMode (L1_press, INPUT_PULLUP);    //10                    // input for L1 keypress
  pinMode (R1_press, INPUT_PULLUP);    //9                        // input for R1 keypress
  pinMode (SEL_press, INPUT_PULLUP);   //8                            // input for SELECT keypress

  pinMode (PAL_pin, OUTPUT);           //7          // OUTPUT for PAL VCC
  pinMode (NTSC_pin, OUTPUT);          //6          //OUTPUT for NTSC VCC

  digitalWrite (12, LOW) ;  //** make sure unused pins in PORT B register are set low   **
  digitalWrite (13, LOW);   //**                                                        **
}
//========================== PAL  =======================

void PAL() {
  digitalWrite (NEW_MODE, !digitalRead (MODE_LATCH));   // After hard press detected on B register @ boot, softwrite new value (smooth transitions)
  delay (2000);                                       //
  if (pin_register == keypress) {     // monitor port B registry
    NTSC();                           // switch to NTSC mode if L1+R1+START+SELECT press detected for 7 seconds straight
  }
  else {
    MODE_LATCH = PAL_MODE;            // set PAL mode after register B monitoring (no videomode change keypress detected)
    PAL();                            // keep looping
  }
}
//========================== NTSC =======================

void NTSC() {
  // set NTSC mode before register B monitoring
  digitalWrite (NEW_MODE, !digitalRead (MODE_LATCH));   // After hard press detected on B register @ boot, softwrite new value (smooth transitions)
  delay (2000);                                         //
  if (pin_register == keypress) {     // monitor port B registry
    PAL();                            // switch to PAL mode if L1+R1+START+SELECT press detected for 7 seconds straight
  }
  else {
    MODE_LATCH = NTSC_MODE;           // set NTSC mode after register B monitoring (no videomode change keypress detected)
    NTSC();                           // keep looping
  }
}

//========================== MAIN =======================
//if video mode keypress detected, boot NTSC, else boot PAL
void loop() {
  if  (pin_register == keypress) {
    MODE_LATCH = NTSC_MODE;
    NTSC();
  }
  else {
    MODE_LATCH = PAL_MODE;
    PAL();
  }
}
