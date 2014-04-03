
#include "armcommon.h"
#include "klcd.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define rgb(r, g, b)  (min(r, 31) << 11) | (min(g, 63) << 5) | min(b, 31)


#include "GLCD.h"
#include <math.h>

typedef struct {
	xQueueHandle toLCD, freed;
} LCDBuf;

// This needs to be here because the params are shallow copied (and must be a pointer)
//    and therefore must not be on the stack. Since we only need one of these, we can just
//    make it file scope.
static LCDBuf lcdCmd;

#ifdef CHECKS
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
		map,
	} type;
} LCDMsg;


static const short backcolor = rgb(0,0,15);
static const short axiscolor = rgb(31,63,0);
static const short forecolor = rgb(31, 63, 31);
static const short thcolor = rgb(0,15,0);

void StartLCD() {
	MAKE_Q(lcdCmd.toLCD, LCDMsg*, 4);
	MAKE_Q(lcdCmd.freed, LCDMsg*, 4);

	StartLCDTask(&lcdCmd);
}
void LCDrefreshMap() {
	reqSig;
	
	LCDMsg* ret;
	RECV(lcdCmd.freed, ret);
	
	ret->type = map;
	LCDcommitBuffer(ret);
}

TextLCDMsg* LCDgetTextBuffer() {
	reqSig;
	
	LCDMsg* ret;
	RECV(lcdCmd.freed, ret);
	
	ret->type = text;
	// Partially initialize the text msg
	ret->text.size = 1;
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
		char* dest = &(msg->data[0]);
		for (i = 0; i < SIGNAL_SAMPLES; i++) {
			dest[i] = sin(count)*100;
			count += 0.15;
		}

		LCDcommitBuffer(msg);
		
		if (++times % 10 == 0) {
			TextLCDMsg* msg = LCDgetTextBuffer();
			msg->line = 10;
			msg->text[0] = '0' + (times/10) % 10;
			msg->text[1] = 0;
			LCDcommitBuffer(msg);
		}
	}
} ENDTASK

void LCDwriteLn(int line, char* data) {
	TextLCDMsg* msg = LCDgetTextBuffer();
	msg->line = line;
	
	msg->size = 0; // could be 1
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

#include "map.h"
void lcdMap() {
	static Map last;
	int a, b;
	GLCD_WindowMax();
	for (a = 0; a < MAP_WIDTH; a++) {
		for (b = 0; b < MAP_WIDTH; b++) {
			int cur = mapTest(a, b);
			if (cur == mapTestMap(&last, a, b)) continue;
			// only change the color if the thing changed so that it goes faster.
			
			int color;
			switch (cur) {
				case 0:	color = rgb(15,63, 0); break;
				case 1: color = rgb(10,33,10); break;
				case 2: color = rgb(10,10,10); break;
				case 3: color = rgb( 31, 0, 0); break;
				default:color = rgb(31, 0, 0); break;
			}
			GLCD_SetTextColor(color);
			
			int x = a*2 + 100;
			int y = b*2 + 0;
			GLCD_PutPixel(x+0, y+0);
			GLCD_PutPixel(x+0, y+1);
			GLCD_PutPixel(x+1, y+0);
			GLCD_PutPixel(x+1, y+1);
		}
	}
	mapGetMap(&last);
}

void lcdSignal(SignalLCDMsg* msg) {
	DBGbit(0, 1);
	GLCD_DisplayString(0, 1,0, (unsigned char*)"0.5V/div");
	GLCD_DisplayString(60, 40,0, (unsigned char*)"20ms/div");
	
	GLCD_WindowMax();

	static short last[LCDWIDTH];
	char *data;
	int x;
	// Autoscale: find the largest value
	/*int max = 0;
	data = &msg->data[0];
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
	}	*/

	float max = 0xFF;
	// Make sure all 0s still displays correctly
	if (max == 0) max = 1;
	else max *= 1; // scale it for debugging

	// Draw Axis
	{
		GLCD_SetTextColor(axiscolor);
		int i;
		for (i = 0; i < LCDWIDTH; i++) {
			GLCD_PutPixel(i, LCDHEIGHT-1);
			if ((i % 32) == 0) {
				GLCD_PutPixel(i, LCDHEIGHT - 2);
				GLCD_PutPixel(i, LCDHEIGHT - 3);
			}
		}
		for (i = 0; i < LCDHEIGHT; i++) {
			int ni = LCDHEIGHT - i;
			GLCD_PutPixel(0, ni);
			
			if ((i % 26) == 0) {
				GLCD_PutPixel(1, ni);
				GLCD_PutPixel(2, ni);
			}
		}
	}
	
	#define LOOP(s) for (x = s, data = &msg->data[s]; x < SIGNAL_SAMPLES; x+=2, data+=2)
	// TODO: Figure out what kind of fp support we have
	#define Y int y = LCDHEIGHT - (int)(*data * (float)(LCDHEIGHT / max));
		 		
	// Use a fancy technique from old interleaved tvs: since writes to the LCD screen are effectively visible to the human
	//    eye, we split it up and rewrite every other line. At fast refresh speeds it eases how it looks (note: could be confirmation
	//    bias, but this at least doesn't have any real negative issues.
	
	GLCD_SetTextColor(backcolor);
	LOOP(0) {
		int l = last[x];
		Y;
		last[x] = y;
		if (l != y) {
			GLCD_PutPixel(x, l);
		}
	}
	
	GLCD_SetTextColor(forecolor);
	LOOP(0) {	   
		GLCD_PutPixel(x, last[x]);
	}

	GLCD_SetTextColor(backcolor);
	LOOP(1) {				   		
		int l = last[x];
		Y;
		last[x] = y;
		if (l != y) {
			GLCD_PutPixel(x, l);
		}
	}
	
	GLCD_SetTextColor(forecolor);
	LOOP(1) {
		GLCD_PutPixel(x, last[x]);
	}


	// Not needed, but may help with things sometimes.
    GLCD_WindowMax();
	
	DBGbit(0, 0);
}

void lcdText(TextLCDMsg*text) {
	//TODO: Add more params to the text message so we can support more things, like colorization and abs location.
	int curLine = text->line;
	//GLCD_ClearLn(curLine,1);
	GLCD_SetTextColor(forecolor);
	// show the text
	GLCD_DisplayString(curLine,0,text->size,(unsigned char *)text->text);
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
	lcdMap();

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
			case map:
				lcdMap();
				break;
			default:
				FATAL(0);
		}
		
		SEND(bufs->freed, msg);
	}
} ENDTASK





