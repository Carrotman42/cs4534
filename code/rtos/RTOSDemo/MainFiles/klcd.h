
#ifndef KLCD_H_INC
#define KLCD_H_INC

void StartLCD();


// Blocks until a buffer is available
SignalLCDMsg* LCDgetSignalBuffer();
TextLCDMsg* LCDgetTextBuffer();

// May block if the LCD is busy
void LCDcommitBuffer(void*aa);
// Will not block
void LCDabortBuffer(void*a);

// DEBUG FUNCTION

// Wraps text to next line.
// NOTE: If this prototype is changed, make sure to change the corresponding prototype in FATAL
void LCDwriteLn(int line, char* data);
#endif





