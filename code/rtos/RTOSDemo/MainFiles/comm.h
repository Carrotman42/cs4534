#ifndef COMM_H
#define COMM_H

#include "armcommon.h"

typedef enum {
	Forward = 0,
	TurnCCW = 1,
	TurnCW = 2,
	Stop = 3,
	StartFrames = 4,
	StopFrames = 5,
	ReadFrames = 6,
	LASTROVER = 7,
} RoverAction;		  

typedef struct {
	RoverAction act;
	char param;
} RoverCmd;

#define MAX_OUT_SIZE 6

// Returns the length. The dest must be at least MAX_OUT_SIZE bytes long.
int copyToBuf(RoverCmd cmd, char* dest);

extern char *Protocol[LASTROVER];

// To be defined somewhere else (etheremu.c, vtI2C.c); must be a threadsafe function which will
//   schedule this command to be written to the PICman. Since it is an async command this does
//   not block. If the command does not send correctly (an ack is not received) then the appropriate
//   error message will be logged.
void asyncWrite(RoverAction, char);

void InitComm();

// To be called by the implementation on data recv. This will take care of parsing and understanding the message sent
//   back to reduce copy-paste.
inline void gotData(RoverAction act, char* outBuf, char* inBuf);

inline RoverAction nextCommand(int* len, char* outBuf);
void registerTickListener(int x);

#define CMD_FUNC(name, cmd) inline void name(char param) { asyncWrite(cmd, param); }
#define CMD_FUNC_NOARG(name, cmd) inline void name() { asyncWrite(cmd, 0); }

CMD_FUNC(moveForward, Forward);
CMD_FUNC(turnCCW, TurnCCW);
CMD_FUNC(turnCW, TurnCW);
CMD_FUNC_NOARG(stop, Stop);

#endif