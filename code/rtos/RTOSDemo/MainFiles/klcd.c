
#include "tasks.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
short rgb(int r, int g, int b) {
	return (min(r, 31) << 11) | (min(g, 63) << 5) | min(b, 31);
}


#include "GLCD.h"
#include "vtUtilities.h"
#include "lcdTask.h"


typedef struct {
	xQueueHandle toLCD, freed;
} LCDBuf;
// The task to display a signal on the LCD screen
TASK_PROTOTYPE(LCDSignalTask, LCDBuf, 1000, tskIDLE_PRIORITY);
#include "lcdTask.h"
TASK_PROTOTYPE(TestTask, LCDBuf, 200, tskIDLE_PRIORITY);

// This needs to be here because the params are shallow copied (and must be a pointer)
//    and therefore must not be on the stack. Since we only need one of these, we can just
//    make it file scope.
static LCDBuf lcdCmd;

void StartSignalTest() {
	MAKE_Q(lcdCmd.toLCD, SignalLCDMsg*, 4);
	MAKE_Q(lcdCmd.freed, SignalLCDMsg*, 4);
	StartLCDSignalTask(&lcdCmd);
	StartTestTask(&lcdCmd);
}

TASK_FUNC(TestTask, LCDBuf, bufs) {
	int count = 0;
	for (;;) {
		SignalLCDMsg* msg;
		RECV(bufs->freed, msg)
		
		int i;
		count++;
		short* dest = &(msg->data[0]);
		for (i = 0; i < SIGNAL_SAMPLES; i++) {
			if (count % 2) {
				dest[i] = SIGNAL_SAMPLES - i;
			} else {
				dest[i] = i;
			}
		}

		SEND(bufs->toLCD, msg);
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





