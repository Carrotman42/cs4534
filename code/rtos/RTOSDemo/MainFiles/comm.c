
#include "comm.h"	 
#include "klcd.h"

#define Prot(name, a, b, pay) \
	enum { \
		name##1 = a, \
		name##2 = b, \
		name##3 = pay, \
	}

// PROT(NAME,  FLAGS, PARAM, HasPayload)
Prot(Forward,     2, 0, 1);
Prot(TurnCCW,     2, 4, 1);
Prot(AdjuCCW,     2, 4, 1);
Prot(TurnCW,      2, 3, 1);
Prot(AdjuCW,      2, 3, 1);
Prot(Stop,        2, 2, 0);
Prot(StartFrames, 4, 0, 0);
Prot(StopFrames,  4, 3, 0);
Prot(ReadFrames,  4, 2, 0);
Prot(TurnAck,     4, 5, 0);

#define P(name) {name##1, name##2, name##3}
static const char protBytes[LASTROVER][3] = {
	P(Forward),
	P(TurnCCW),
	P(AdjuCCW),
	P(TurnCW),
	P(AdjuCW),
	P(Stop),
	P(StartFrames),
	P(StopFrames),
	P(ReadFrames),
	P(TurnAck),
};

#define copyToPay(a,b,pay) \
	*dest++ = a; \
	*dest++ = b; \
	*dest++ = 0; \
	*dest++ = 1; \
	*dest++ = a+b+1+pay; \
	*dest++ = pay;
#define copyTo(a,b) \
	*dest++ = a; \
	*dest++ = b; \
	*dest++ = 0; \
	*dest++ = 0; \
	*dest++ = a+b;
	

#define Case(name) \
	case name: \
		if (name##3) { \
			copyToPay(name##1, name##2, cmd.param); \
			return 6; \
		} else { \
			copyTo(name##1, name##2); \
			return 5; \
		}

// Returns the length. The dest must be at least MAX_OUT_SIZE bytes long.
int copyToBuf(RoverCmd cmd, char* dest) {
	switch (cmd.act) {
		Case(Forward);
		Case(TurnCCW);
		Case(AdjuCCW);
		Case(TurnCW);
		Case(AdjuCW);
		Case(Stop);
		Case(StartFrames);
		Case(StopFrames);
		Case(ReadFrames);
		Case(TurnAck);
		default:
			FATAL(cmd.act);
			return -1;
	}
}

#if ETHER_EMU==1
#define ETHER_TASKS

#else
#define I2C_TASKS

#include "vti2c.h"
static vtI2CStruct vtI2C0;
#endif

#include "tasks.h"

static xQueueHandle toRover;
void InitComm() {
	MAKE_Q(toRover, RoverCmd, 4);
#if ETHER_EMU==1
#else
	FAILIF(vtI2CInit(&vtI2C0,0,mainI2CMONITOR_TASK_PRIORITY,I2C_Stack) != vtI2CInitSuccess);
#endif
}

#include "brain_rover.h"
#include "map.h"

// Keep track of how many times we've seen an invalid ReadFrames response
static int invalid = 0;
// Keep track of if we're turning: 
//   -1 means it was last turning right, 1 means it was last turning left, 0 means it isn't turning.						   
static int turning = 0;


 
// Will check whether the message header matches an error message. In this case it prints
//   it on the LCD. TODO: Tell the webserver that something is wrong.
inline void checkError(char* ret) {
	if (!(ret[0] & ERROR_FLAG)) {
		return;
	}
	
	bBuf(30);
	bStr("ERR: ");
	bByte(ret[1]);
	bPrint(6);
	
	dbg(Error, ret[1]);
}

// outBuf was the message written to the picman
// inBuf is the message that was just read from the picman
inline void gotData(RoverAction last, char* ret) {
	// Remove any priorities because we treat all messages the same
	ret[0] = ret[0] & ~HIGH_PRIORITY;
	switch (last) {
		case ReadFrames: {
			checkError(ret);
			if (ret[0] & FRAME_NOT_VALID) {
				invalid++;
				break;
			}
			invalid = 0;
			mapReportNewFrame(ret[0] & COLOR_SENSED, turning != 0, &ret[HEADER_MEMBERS]);
			break;
		}
		case TurnAck: {
			if (!turning) {
				LCDwriteLn(10, "Got TurnAck without a 'turning' value!");
				return;
			}
			
			// A payload of 0 means still turning, 1 means done turning
			if (ret[HEADER_MEMBERS]) {
				if (turning == 4) {
					turning = 0;
				}
				mapReportTurn(turning);
				turning = 0;
			}
			break;
		}
		default: { // An async cmd
			// Check the ack:
			char tmp[MAX_OUT_SIZE];
			RoverCmd cmd;
			cmd.act = last;
			//cmd.param = 0; doesn't matter
			copyToBuf(cmd, tmp);
			if ((tmp[0] | ACK_FLAG) != ret[0]) {
				// Error in ack!
				ReportInvalidResponse(last, tmp, ret);
			} else if (last == TurnCCW) {
				turning = 1;
			} else if (last == TurnCW) {
				turning = -1;
			} else if (last == AdjuCCW || last == AdjuCW) {
				turning = 4;
			}
		}
			break;
	}
	
	
}


inline RoverAction nextCommand(int* len, char* outBuf) {
	RoverCmd cmd;
	if (!TRY_RECV(toRover, cmd)) {
		static int swap = 0;
		
		if (turning) {
			cmd.act = (swap ^= 1) ? TurnAck : ReadFrames;
		} else if (invalid > 10) {
			invalid = 0;
			cmd.act = StartFrames;
		} else {
			cmd.act = ReadFrames;
		}
		SLEEP(150);
	} else {
		dbg(SendCmd, cmd.act);
	}
	
	/*{
		RECV(toRover, cmd);
		dbg(SendCmd, cmd.act);
	}*/
	
	*len = copyToBuf(cmd, outBuf);
	return cmd.act;
}

inline void asyncWrite(RoverAction act, char param) {
	// TODO: split this up so that I2C and ethernet can be more optimized maybe
	RoverCmd cmd;
	cmd.act = act;
	cmd.param = param;
	SEND(toRover, cmd);
}

inline void registerTickListener(int x) {
	mapRegisterTick(x);
}






