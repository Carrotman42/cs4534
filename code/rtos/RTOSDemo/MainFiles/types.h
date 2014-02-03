
// Types and constants file, to try to keep things organized. May split this into multiple files
//    later if it becomes unwieldy
#ifndef TYPES_H_INC
#define TYPES_H_INC

#define LCDWIDTH 320
#define LCDHEIGHT 240

#define SIGNAL_SAMPLES (LCDWIDTH)
typedef struct {
	short data[SIGNAL_SAMPLES];
} SignalLCDMsg;

#endif