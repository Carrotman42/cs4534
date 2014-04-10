
#include "armcommon.h"
#include "lpc17xx_gpio.h"
#include "kdbg.h"

#define b(x) (1 << x)
void InitDBG() {
	GPIO_SetDir(0, b(0) | b(1), 1);
}

void DBGbit(unsigned value, int on) {
	if (value >= 2) {
		FATALSTR("Too many bits required for debug function", value);
	}
	if (on) {
		GPIO_SetValue(0, 1<<value);
	} else {
		GPIO_ClearValue(0, 1<<value);
	}
}

#include "klcd.h"
void ReportDroppedMsg(int len, char* msg) {
	//aBuf(b, 100);
	LCDwriteLn(14, "DROPPED MSG!!");
}

#define D(a) \
	bByte(a[0]); \
	bChar(' '); \
	bByte(a[1]); \
	bChar(' '); \
	bByte(a[3]);
void ReportInvalidResponse(int last, char* orig, char* resp) {
	bBuf(40);
	bStr("Bad resp to cmd ");
	bByte(last);
	bStr(" (");
	D(orig);
	bStr("): ");
	D(resp);
	bPrint(14);
}

DbgRecord dbgs[500];
int curdbg = 0;

void dbg(DebugType kind, int param) {
	if (curdbg >= (sizeof(dbgs) / sizeof(dbgs[0]))) {
		if (dbgs[0].k != Overrun) {
			dbgs[0].k = Overrun;
			dbgs[0].p = 0;
		} else {
			dbgs[0].p++;
		}
		curdbg = 1;
	}
	DbgRecord* dest = &dbgs[curdbg];
	// I believe we have sequential consistency on this thing, so the order here matters.
	dest->k = kind;
	dest->p = param;
	curdbg++;
}
DbgRecord* dbgGet(int*len) {
	*len = curdbg;
	return &dbgs[0];
}










