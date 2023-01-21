/*
 _____  _                                       ___    ___      _             _  ___    ___   
(  _  )( )_                                    (  _`\ (  _`\  /' )           (_)(  _`\ |  _`\ 
| (_) || ,_)  ___ ___     __     __     _ _    | |_) )| (_(_)(_, | ______    | || ( (_)| (_) )
|  _  || |  /' _ ` _ `\ /'__`\ /'_ `\ /'_` )   | ,__/'`\__ \   | |(______)   | || |___ | ,  / 
| | | || |_ | ( ) ( ) |(  ___/( (_) |( (_| |   | |    ( )_) |  | |           | || (_, )| |\ \ 
(_) (_)`\__)(_) (_) (_)`\____)`\__  |`\__,_)   (_)    `\____)  (_)           (_)(____/'(_) (_)
                              ( )_) |                                                         
                               \___/'                                                         VajskiDs consoles 2023

Short Reset: L2 + R2 + SELECT + START (back to current selected game)
Long Reset:  L2 + R2 + SELECT + CROSS (back to Xstation menu)
D7 = Reset
D8 = CTRL Data
D9 = CTRL Clk


V1.1: 

Fixed analogue Mode. (rushed release, check the old code - it was pretty safe to assume that it was working :P)
Only check high nibble in byte 2 (array element 1) for device mode now. 

Added support for SCPH1180 controller (Large non dual shock with analogue joysticks, analogue button goes from blank/red/green)
- This controller has different mappings, so added short and reset combo triggers (same button combos, different triggers).


These Device mode only combined with reset combos for extra security against false reset flags as the BUS is shared with the memory card (as we could just check the button input bytes 3 & 4 )
Left debug printing in the code and some other minor changes.

If you want to change or add reset combos you can simply comment out the reset lines, run the console with the UART connected (leave power d/c)
and see your desired combos printed on the serial monitor (just hold the combo), change / add the combo accordingly in the code. 

*/


#define resetPin 7              //PD7
#define clk 1                   //PB1
#define controllerDataPin 0     //PB0
#define clockishigh bitRead(PINB, clk) == HIGH
#define ControllerDataStateIdling bitRead(PINB, controllerDataPin) == 1
#define ControllerModes NibbleStorage == 0x7 || NibbleStorage == 0x4 || NibbleStorage == 0x5        // [7h = Analogue / 4h = Digital / 5h = SCPH1180 Green LED]
#define SHORTRST bitSet(DDRD, resetPin), delay (200), bitClear(DDRD, resetPin)
#define LONGRST bitSet(DDRD, resetPin), delay (2000), bitClear(DDRD, resetPin)                      // F67E / FE3E combos are for SCPH1180 Green LED Mode
#define shortresetcombo ControllerByteArray[3] == 0xF6 && ControllerByteArray[4] == 0xFC || ControllerByteArray[3] == 0xF6 && ControllerByteArray[4] == 0x7E
#define longresetcombo  ControllerByteArray[3] == 0xFE && ControllerByteArray[4] == 0xBC || ControllerByteArray[3] == 0xFE && ControllerByteArray[4] == 0x3E
uint8_t ControllerByteArray[5] = { 0x00 };
bool controllerconnected;



void setup() {

  Serial.begin(115200);                                     // enable serial monitor as a type of debug console
  pinMode(7, INPUT), pinMode(8, INPUT), pinMode(9, INPUT);  // They should default to inputs anyway
  bitClear (PORTD, resetPin);                               // reset pin low (should default to low when switched to an output anyway)
}


void print() {  //  debug print
  
    for (auto debugprint = 0; debugprint < 5; debugprint++) {
    if (ControllerByteArray[debugprint] == 0x00 || ControllerByteArray[debugprint] < 0x10) Serial.print("0");
    Serial.print(ControllerByteArray[debugprint], HEX);
    Serial.print("  ");
    Serial.flush();
    if (debugprint == 4) Serial.print("\n");
  }

}



void checkdevicemodenibble(){

  uint8_t NibbleStorage = 0x00;

      for (auto nibblecheck = 0; nibblecheck < 8; nibblecheck++){
  
            if (bitRead (ControllerByteArray[1], nibblecheck) == 1)
                  bitWrite(NibbleStorage, nibblecheck-4, 1);
                    else bitWrite(NibbleStorage, nibblecheck-4, 0);
                   
}                     
 if (ControllerModes && ControllerByteArray[0] == 0xFF && ControllerByteArray[2] == 0x5A) controllerconnected = true;
 
//Serial.println (NibbleStorage, HEX);
}



void capturepackets() //Main FOR = bytes, nested FOR = bits
{

        static uint8_t ControllerBytes = 0x00;
       
            for (auto ControllerBytePosition = 0; ControllerBytePosition < 5; ControllerBytePosition++) {

                  for (auto bitPosition = 0; bitPosition < 8; bitPosition++) {                  // Clocks gone low, transmission started, ready to capture 8 bits
                        
                        while(clockishigh);                                                       // pause  
                                                     
                            if (ControllerDataStateIdling) bitSet(ControllerBytes, bitPosition);  // grab the bits on the falling edge
                                else bitClear (ControllerBytes, bitPosition);
                               
                                    while(!clockishigh);                                            // Wait for the clock to rise again and re-loop, perfect reads *everytime* !
                                                
                                                                                                                                                                                      
                                                                                }

    ControllerByteArray[ControllerBytePosition] = ControllerBytes;  // store the bits into a byte
    
                                                                                                    }

}



        void loop() {

          noInterrupts();
          capturepackets();
          interrupts();
          print();
          checkdevicemodenibble();
          
                if (controllerconnected && shortresetcombo) SHORTRST;                  // SHORT RESET

                if (controllerconnected && longresetcombo) LONGRST;                    // LONG RESET

          controllerconnected = false;                                                 // check on each poll that a controllers still connected
                  
}

