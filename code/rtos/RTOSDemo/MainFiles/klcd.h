
#ifndef KLCD_H_INC
#define KLCD_H_INC

void LCDinit();

void StartSignalTest();
void StartLCD();

// Blocks until a buffer is available
SignalLCDMsg* LCDgetSignalBuffer();
// May block if the LCD is busy
void LCDcommitSignalBuffer(SignalLCDMsg*aa);
// Will not block
void LCDabortSignalBuffer(SignalLCDMsg*a);


#endif