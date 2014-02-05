
#ifndef KLCD_H_INC
#define KLCD_H_INC

void LCDinit();

void StartSignalTest();
void StartLCD();


// Blocks until a buffer is available
SignalLCDMsg* LCDgetSignalBuffer();
TextLCDMsg* LCDgetTextBuffer();

// May block if the LCD is busy
void LCDcommitBuffer(void*aa);
// Will not block
void LCDabortBuffer(void*a);

// DEBUG FUNCTION
// Doesn't bounds check
void LCDwriteLn(int line, char* data);
#endif





