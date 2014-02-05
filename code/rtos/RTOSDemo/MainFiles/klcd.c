
#include "common.h"
#include "klcd.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define rgb(r, g, b)  (min(r, 31) << 11) | (min(g, 63) << 5) | min(b, 31)


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

#ifdef CHECKS
// Note: this doesn't actually work to check to make sure things were initted
//   because C doesn't initialize variables to be a specific value. But just in
//   case I look into making them zeroed out initially I'm leaving them here.
#define reqSig FAILIF(lcdCmd.toLCD == NULL && lcdCmd.freed == NULL)
#else
#define reqSig
#endif

#define LCD_TASKS
#include "tasks.h"



typedef struct {
	union {
		SignalLCDMsg signal;
		TextLCDMsg text;
	};
	enum {
		signal,
		text,
	} type;
} LCDMsg;


static const short backcolor = rgb(0,0,15);
static const short forecolor = rgb(31, 63, 31);
static const short thcolor = rgb(0,15,0);

void StartSignalTest() {
	StartLCD();																							 
	StartTestSignalTask();
}
void StartLCD() {
	MAKE_Q(lcdCmd.toLCD, LCDMsg*, 4);
	MAKE_Q(lcdCmd.freed, LCDMsg*, 4);

	StartLCDTask(&lcdCmd);
}

TextLCDMsg* LCDgetTextBuffer() {
	reqSig;
	
	LCDMsg* ret;
	RECV(lcdCmd.freed, ret);
	
	ret->type = text;
	return &ret->text;
}

SignalLCDMsg* LCDgetSignalBuffer() {
	reqSig;
	
	LCDMsg* ret;
	RECV(lcdCmd.freed, ret);
	
	ret->type = signal;
	return &ret->signal;
}

void LCDcommitBuffer(void*in) {
	reqSig;
	
	// Make sure we can put it back in the same way
	ASSERT(&(((LCDMsg*)0)->signal) == (SignalLCDMsg*)0);
	ASSERT(&(((LCDMsg*)0)->text) == (TextLCDMsg*)0);
	
	SEND(lcdCmd.toLCD, in);
}
void LCDabortBuffer(void*in) {
	reqSig;
	
	SEND(lcdCmd.freed, in);
}

TASK_FUNC_NOARG(TestSignalTask) {
	double count = 0;
	int times = 0;
	for (;;) {
		SignalLCDMsg* msg = LCDgetSignalBuffer();
		
		int i;
		short* dest = &(msg->data[0]);
		for (i = 0; i < SIGNAL_SAMPLES; i++) {
			dest[i] = sin(count)*100;
			count += 0.15;
		}

		LCDcommitBuffer(msg);
		
		if (++times % 10 == 0) {
			TextLCDMsg* msg = LCDgetTextBuffer();
			msg->line = 2;
			msg->text[0] = '0' + (times/10) % 10;
			msg->text[1] = 0;
			LCDcommitBuffer(msg);
			// Fast way of doing that, but less efficient due to copying:
			LCDwriteLn(6, "A string");
		}
	}
} ENDTASK


void LCDwriteLn(int line, char* data) {
	TextLCDMsg* msg = LCDgetTextBuffer();
	msg->line = line;
	int i = 0;
	while (1) {
		char d = *data++;
		msg->text[i++] = d;
		if (d == 0) {
			break;
		}
	}
	
	LCDcommitBuffer(msg);
}

void lcdSignal(SignalLCDMsg* msg) {
	GLCD_WindowMax();
	// It's okay to have static vars: we're guarenteed to always be in the same task.
	//   ps: I hope the memory model of freertos guarentees that... It seems appropriate
	//          to assume.
	static short last[LCDWIDTH];
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

	#define LOOP(s) for (x = s, data = &msg->data[s]; x < SIGNAL_SAMPLES; x+=2, data+=2)
	// TODO: Figure out what kind of fp support we have
	#define Y int y = *data * (LCDHEIGHT/2) / max + LCDHEIGHT/2;
		 		
	// Use a fancy technique from old interleaved tvs: since writes to the LCD screen are effectively visible to the human
	//    eye, we split it up and rewrite every other line. At fast refresh speeds it eases how it looks (note: could be confirmation
	//    bias, but this at least doesn't have any real negative issues.
	GLCD_SetTextColor(backcolor);
	LOOP(0) {				   			   
		GLCD_PutPixel(x, last[x]);
	}
	
	GLCD_SetTextColor(forecolor);
	LOOP(0) {
		Y;
		last[x] = y;			   
		GLCD_PutPixel(x, y);
	}

	GLCD_SetTextColor(backcolor);
	LOOP(1) {				   			   
		GLCD_PutPixel(x, last[x]);
	}
	
	GLCD_SetTextColor(forecolor);
	LOOP(1) {
		Y;
		last[x] = y;			   
		GLCD_PutPixel(x, y);
	}


	// Not needed, but may help with things sometimes.
    GLCD_WindowMax();
}

void lcdText(TextLCDMsg*text) {
	//TODO: Add more params to the text message so we can support more things, like colorization and abs location.
	int curLine = text->line;
	//GLCD_ClearLn(curLine,1);
	// show the text
	GLCD_DisplayString(curLine,0,1,(unsigned char *)text->text);
}

TASK_FUNC(LCDTask, LCDBuf, bufs) {
	GLCD_Init();
	GLCD_SetTextColor(forecolor);
	// May not need to set here?
	GLCD_SetBackColor(thcolor);
	GLCD_Clear(backcolor);

	LCDMsg allocd[2];
	int i;
	for (i = 0; i < sizeof allocd / sizeof(LCDMsg); i++) {
	 	LCDMsg* h = &allocd[i];		
		SEND(bufs->freed, h);
	}

	while (1) {
		LCDMsg* msg;
		RECV(bufs->toLCD, msg);
		
		switch (msg->type) {
			case signal:
				lcdSignal(&msg->signal);
				break;
			case text:
				lcdText(&msg->text);
				break;
			default:
				FATAL(0);
		}
		
		SEND(bufs->freed, msg);
	}
} ENDTASK





