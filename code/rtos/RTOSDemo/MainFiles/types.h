
// Types and constants file, to try to keep things organized. May split this into multiple files
//    later if it becomes unwieldy
#ifndef TYPES_H_INC
#define TYPES_H_INC

#define LCDWIDTH 320
#define LCDHEIGHT 240

// NOTE: SIGNAL_SAMPLES *MUST* remain equal to LCDWIDTH because the interleaving
//   algorithm for the lcd depends on it.
#define SIGNAL_SAMPLES (LCDWIDTH)
typedef struct {
	short data[LCDWIDTH];
} SignalLCDMsg;

#define CHARS 320/16
typedef struct {
	unsigned char line;
	// Can be small (0) or large (1)
	unsigned char size : 1;
	// Must be 0 terminated
	char text[CHARS + 1];
} TextLCDMsg;

#endif