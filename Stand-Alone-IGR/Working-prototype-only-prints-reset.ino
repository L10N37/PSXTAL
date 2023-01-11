/*
 _____  _                                       ___    ___      _             _  ___    ___   
(  _  )( )_                                    (  _`\ (  _`\  /' )           (_)(  _`\ |  _`\ 
| (_) || ,_)  ___ ___     __     __     _ _    | |_) )| (_(_)(_, | ______    | || ( (_)| (_) )
|  _  || |  /' _ ` _ `\ /'__`\ /'_ `\ /'_` )   | ,__/'`\__ \   | |(______)   | || |___ | ,  / 
| | | || |_ | ( ) ( ) |(  ___/( (_) |( (_| |   | |    ( )_) |  | |           | || (_, )| |\ \ 
(_) (_)`\__)(_) (_) (_)`\____)`\__  |`\__,_)   (_)    `\____)  (_)           (_)(____/'(_) (_)
                              ( )_) |                                                         
                               \___/'                                                         VajskiDs consoles 2023
*/
// Working prototype, prints reset when the combination - L1 + L2 + R1 + R2 + SEL + START is pressed in digital or analogue mode
// Info all here: https://store.curiousinventor.com/guides/PS2/, though i get 0x54 as byte 3, not 0x5A
// Short and long reset combos to be decided, then actual reset functionality added
// also fail safe it with an extra byte to confirm its coming from a controller and not a memory card


#define clk 1
#define controllerDataPin 0
#define clockishigh bitRead(PINB, clk) == HIGH
#define ControllerDataState bitRead(PINB, controllerDataPin) == 1
uint8_t ControllerByteArray[10] = { 0x00 };



void setup() {

  Serial.begin(115200);                  // enable serial monitor as a type of debug console
  pinMode(8, INPUT), pinMode(9, INPUT);  // They should default to inputs anyway
}


void  printDigitalMode() {  // subq byte capture debug serial printing (6 pulse trains for digital 10 for analogue)

  for (int debugprint = 0; debugprint < 6; debugprint++) {
    if (ControllerByteArray[debugprint] == 0x00 || ControllerByteArray[debugprint] < 0x10) Serial.print("0");
    Serial.print(ControllerByteArray[debugprint], HEX);
    Serial.print("  ");
    Serial.flush();
    if (debugprint == 5) Serial.print("\n");
  }
}

void  printAnalogueMode() {  // subq byte capture debug serial printing (6 pulse trains for digital 10 for analogue)

  for (int debugprint = 0; debugprint < 10; debugprint++) {
    if (ControllerByteArray[debugprint] == 0x00 || ControllerByteArray[debugprint] < 0x10) Serial.print("0");
    Serial.print(ControllerByteArray[debugprint], HEX);
    Serial.print("  ");
    Serial.flush();
    if (debugprint == 9) Serial.print("\n");
  }
}




void capturepackets() {

  uint8_t ControllerBytePos = 0;
  uint8_t ControllerBytes = 0x00;

 

  while (ControllerBytePos < 10) {

     noInterrupts();
     while (clockishigh);  //pause

    for (int bitPosition = 0; bitPosition < 8; bitPosition++) {  //capture 8 bits

      while (!clockishigh);  //pause
      

      if (ControllerDataState) bitSet(ControllerBytes, bitPosition);  // grab our bit now on the next high
      else bitClear(ControllerBytes, bitPosition);
    }

    ControllerByteArray[ControllerBytePos] = ControllerBytes;         // store the bits into a byte
    ControllerBytePos++;
  }

}



void loop() {

  capturepackets();
  interrupts();
  if (ControllerByteArray[1] == 0x41)  printDigitalMode();  
  else if (ControllerByteArray[1] == 0x67) printAnalogueMode();

  if (ControllerByteArray[3] == 0xEC && ControllerByteArray[4] == 0xE0) {Serial.print ("RESET");}

  delay(500); //slow down the polling, twice a second

}
