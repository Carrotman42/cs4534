
#ifndef KDBG_H_INC
#define KDBG_H_INC

void InitDBG();

// Will write a number in binary. Will fail if the number of bits required was
//    not requested in InitDGB(). Note that this value is hardcoded.
void DBGbit(unsigned bitnum, int on);
void ReportInvalidResponse(int last, char* orig, char* resp);

typedef enum {
	Init = 1,
	SendCmd = 2,
	Overrun = 3,
	Lap = 4,
	Error = 5,
	StateChange = 6,
	// Should accompany a StateChange, or else the output message won't make sense.
	NewEvent = 7,
	InvalidEvent = 8,
} DebugType;

typedef struct {
	char k;
	char p;
} DbgRecord;

void dbg(DebugType kind, int param);
DbgRecord* dbgGet(int*len);

#endif