/*
 _____  _                                       ___    ___      _             _  ___    ___   
(  _  )( )_                                    (  _`\ (  _`\  /' )           (_)(  _`\ |  _`\ 
| (_) || ,_)  ___ ___     __     __     _ _    | |_) )| (_(_)(_, | ______    | || ( (_)| (_) )
|  _  || |  /' _ ` _ `\ /'__`\ /'_ `\ /'_` )   | ,__/'`\__ \   | |(______)   | || |___ | ,  / 
| | | || |_ | ( ) ( ) |(  ___/( (_) |( (_| |   | |    ( )_) |  | |           | || (_, )| |\ \ 
(_) (_)`\__)(_) (_) (_)`\____)`\__  |`\__,_)   (_)    `\____)  (_)           (_)(____/'(_) (_)
                              ( )_) |                                                         
                               \___/'                                                         VajskiDs consoles 2023
V1.00


Short Reset: L2 + R2 + SELECT + START (back to current selected game)

Long Reset:  L2 + R2 + SELECT + CROSS (back to Xstation menu)

D7 = Reset
D8 = CTRL Data
D9 = CTRL Clk

*/


#define resetPin 7              //PD7
#define clk 1                   //PB1
#define controllerDataPin 0     //PB0
#define clockishigh bitRead(PINB, clk) == HIGH
#define ControllerDataStateIdling bitRead(PINB, controllerDataPin) == 1
#define analogueMode ControllerByteArray[0] == 0xFF && ControllerByteArray[1] == 0x41 && ControllerByteArray[2] == 0x5A
#define digitalMode ControllerByteArray[0] == 0xFF && ControllerByteArray[1] == 0x73 && ControllerByteArray[5] == 0xFF && ControllerByteArray[2] == 0x5A
#define SHORTRST bitSet(DDRD, resetPin), delay (200), bitClear(DDRD, resetPin)
#define LONGRST bitSet(DDRD, resetPin), delay (2000), bitClear(DDRD, resetPin)
uint8_t ControllerByteArray[6] = { 0x00 };



void setup() {

  Serial.begin(115200);                                     // enable serial monitor as a type of debug console
  pinMode(7, INPUT), pinMode(8, INPUT), pinMode(9, INPUT);  // They should default to inputs anyway
  bitClear (PORTD, resetPin);                               // reset pin low (should default to low when switched to an output anyway)
}


void capturepackets() //Main FOR = bytes, nested FOR = bits
{

      static uint8_t ControllerBytes = 0x00;
      noInterrupts();
   
        
            for (int ControllerBytePosition = 0; ControllerBytePosition < 6; ControllerBytePosition++) {

                while(clockishigh);   //pause (clock high idle between byte transmissions)
                  
                    for (auto bitPosition = 0; bitPosition < 8; bitPosition++) {                              // Clocks gone low, transmission started, ready to capture 8 bits
                        
                        while(clockishigh);   //pause (clock high idle between transmissions)                 // This is needed for every bit after the first
                                                     
                            if (ControllerDataStateIdling) bitSet(ControllerBytes, bitPosition);              // grab the bits
                                else bitClear(ControllerBytes, bitPosition);

                                  while(!clockishigh);                                                        // Wait for the clock to rise again and re-loop, perfect reads *everytime* !
                                                
                                                                                                                                                                                      
                                                                                }

    ControllerByteArray[ControllerBytePosition] = ControllerBytes;  // store the bits into a byte
                                                                                                    }

}



        void loop() {

          capturepackets();
          interrupts();
          
          if (analogueMode||digitalMode && ControllerByteArray[3] == 0xF6 && ControllerByteArray[4] == 0xFC) SHORTRST; // SHORT RESET

          if (analogueMode||digitalMode && ControllerByteArray[3] == 0xFE && ControllerByteArray[4] == 0xBC) LONGRST;  // LONG RESET

          delay(250);  // slow down the polling
        }



