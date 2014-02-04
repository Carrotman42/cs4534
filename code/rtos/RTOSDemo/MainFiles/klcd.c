
#include "common.h"
#include "klcd.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
short rgb(int r, int g, int b) {
	return (min(r, 31) << 11) | (min(g, 63) << 5) | min(b, 31);
}


#include "GLCD.h"
#include "vtUtilities.h"
#include <math.h>

typedef struct {
	xQueueHandle toLCD, freed;
} LCDBuf;

// This needs to be here because the params are shallow copied (and must be a pointer)
//    and therefore must not be on the stack. Since we only need one of these, we can just
//    make it file scope.
static LCDBuf lcdCmd;

static xQueueHandle lcdTxt;

#ifdef CHECKS
// Note: this doesn't actually work to check to make sure things were initted
//   because C doesn't initialize variables to be a specific value. But just in
//   case I look into making them zeroed out initially I'm leaving them here.
#define reqSig FAILIF(lcdCmd.toLCD == NULL && lcdCmd.freed == NULL)
#define reqText FAILIF(lcdTxt == NULL)
#else
#define reqSig
#endif

#define LCD_TASKS
#include "tasks.h"




void StartTextTest() {
	MAKE_Q(lcdTxt, TextLCDMsg, 4)
}


void StartSignalTest() {
	MAKE_Q(lcdCmd.toLCD, SignalLCDMsg*, 4);
	MAKE_Q(lcdCmd.freed, SignalLCDMsg*, 4);

	StartLCDSignalTask(&lcdCmd);
	StartTestTask(&lcdCmd);
}

SignalLCDMsg* LCDgetSignalBuffer() {
	reqSig;
	
	SignalLCDMsg* ret;
	RECV(lcdCmd.freed, ret);
	return ret;
}
void LCDcommitSignalBuffer(SignalLCDMsg*in) {
	reqSig;
	
	SEND(lcdCmd.toLCD, in);
}
void LCDabortSignalBuffer(SignalLCDMsg*in) {
	reqSig;
	
	SEND(lcdCmd.freed, in);
}

TASK_FUNC(TestTask, LCDBuf, bufs) {
	double count = 0;
	for (;;) {
		SignalLCDMsg* msg = LCDgetSignalBuffer();
		
		int i;
		short* dest = &(msg->data[0]);
		for (i = 0; i < SIGNAL_SAMPLES; i++) {
			dest[i] = sin(count)*100;
			count += .25;
		}

		LCDcommitSignalBuffer(msg);
	}
} ENDTASK

TASK_FUNC(LCDSignalTask, LCDBuf, bufs) {
	GLCD_Init();
	const short backcolor = rgb(0,0,15);
	const short forecolor = rgb(31, 63, 31);
	const short thcolor = rgb(0,15,0);
	GLCD_SetTextColor(forecolor);
	// May not need to set here?
	GLCD_SetBackColor(thcolor);
	GLCD_Clear(backcolor);

	SignalLCDMsg allocd, *h = &allocd;
	SEND(bufs->freed, h);
	int curX = 0;
	short last[LCDWIDTH];
	while (1) {
		SignalLCDMsg* msg;
		RECV(bufs->toLCD, msg);
		
		// Autoscale: find the largest value
		int max = 0;
		short *data = &msg->data[0];
		int x;
		for (x = 0; x < SIGNAL_SAMPLES; x++, data++) {
			short val = *data;
			if (val > 0) {
				if (val > max) {
					max = val;
				}
			} else {
				val = -val;
				if (val > max) {
					max = val;
				}
			}
		}
		
		// Make sure all 0s still displays correctly
		if (max == 0) max = 1;
		else max *= 5; // scale it for debugging
		data = &msg->data[0]; 

		if (curX + SIGNAL_SAMPLES > LCDWIDTH) {
			curX = 0;
		}

		for (x = 0; x < SIGNAL_SAMPLES; x++, data++, curX++) {
			// TODO: Figure out what kind of fp support we have
			int y = *data * (LCDHEIGHT/2) / max + LCDHEIGHT/2;
			
			GLCD_SetTextColor(backcolor);
			GLCD_PutPixel(curX, last[x]);
			last[x] = y;
			GLCD_SetTextColor(forecolor);
			GLCD_PutPixel(curX, y);
		}
		
		SEND(bufs->freed, msg);
	}
} ENDTASK





