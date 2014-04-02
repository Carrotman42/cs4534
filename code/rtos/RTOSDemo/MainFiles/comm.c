
#include "comm.h"

#define Prot(name, a, b, pay) \
	enum { \
		name##1 = a, \
		name##2 = b, \
		name##3 = pay, \
	}

// PROT(NAME,  FLAGS, PARAM, HasPayload)
Prot(Forward,     2, 0, 1);
Prot(TurnCCW,     2, 4, 1);
Prot(TurnCW,      2, 3, 1);
Prot(Stop,        2, 2, 0);
Prot(StartFrames, 4, 0, 0);
Prot(StopFrames,  4, 3, 0);
Prot(ReadFrames,  4, 2, 0);
Prot(TurnAck,     4, 5, 0);

#define P(name) {name##1, name##2, name##3}
static const char protBytes[LASTROVER][3] = {
	P(Forward),
	P(TurnCCW),
	P(TurnCW),
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
		Case(TurnCW);
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

/*
// Returns 0 when that was the last byte to be written
int writeByte(RoverCmd cmd, int byteIndex, char* dest) {
	switch (byteIndex) {
	case 0:
	case 1:
		*dest = protBytes[cmd.act][byteIndex];
		return 1;
	case 2:
		*dest = 0; //msgid
		return 1;
	case 3:
		*dest = protBytes[cmd.act][2];
		return 1;
	case 4: {
		char temp = protBytes[cmd.act][3];
		if (!protBytes[cmd.act][2]) {
			*dest = temp;
			return 0; // Done!
		}
		
		*dest = temp + cmd.param;
		return 1; // Still have one more byte: the payload itself
	}
	case 5: // If they ask for the 5th one, assume that this msg has a payload and send it over
		*dest = cmd.param;
		return 0; // All commands have at most one payload byte
	}
	FATAL(byteIndex);
	return 0;
}*/

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
	StartEtherEmu();
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


#include "klcd.h"
 
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
}

// outBuf was the message written to the picman
// inBuf is the message that was just read from the picman
inline void gotData(RoverAction last, char* ret) {
	// TODO: Check checksum maybe
	
	switch (last) {
		case ReadFrames: {
			checkError(ret);
			if (ret[0] & FRAME_NOT_VALID) {
				invalid++;
				break;
			}
			invalid = 0;
			mapReportNewFrame(&ret[HEADER_MEMBERS]);
			break;
		}
		case TurnAck: {
			if (!turning) {
				LCDwriteLn(10, "Got TurnAck without a 'turning' value!");
				return;
			}
			// A payload of 0 means still turning, 1 means done turning
			if (ret[HEADER_MEMBERS]) {
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
			}
		}
			break;
	}
	
	
}


inline RoverAction nextCommand(int* len, char* outBuf) {
	RoverCmd cmd;
	//SLEEP(1000);
	if (!TRY_RECV(toRover, cmd)) {
		char *dbg;
		if (turning) {
			cmd.act = TurnAck;
			dbg = "Send TurnAck";
		} else if (invalid > 10) {
			invalid = 0;
			cmd.act = StartFrames;
			dbg = "Send StartFrames";
		} else {
			// Right now I think it's best not to have a delay since we want frame data as fast as we can get it.
			cmd.act = ReadFrames;
			dbg = "Send ReadFrames";
		}
		LCDwriteLn(4, dbg);
	}
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






