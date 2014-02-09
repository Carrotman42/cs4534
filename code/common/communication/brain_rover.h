
#ifndef BRAIN_ROVER_H_INC
#define BRAIN_ROVER_H_INC

#include "../sensor_types.h"
#include "../../pic/F3/src/src/my_i2c.h"


// You can use this struct because it is constant length
typedef struct {
	uint8 flags;
	uint8 sensorMask;
} BrainMsg;

typedef struct {
	int (*adFunc)(sensorADData* data, int len);
} RoverMsgRouter;

// THE FOLLOWING ARE FLAGS FOR BrainMsg.flags
#define SENSOR_REQ 1
// END FLAGS

// Should only use the functions prototyped out here
int packADData(sensorADData* data, int len, char* out, int maxout);
int unpackRoverMsg(char* in, int len, RoverMsgRouter* handler);
void packBrainMsgRequest(BrainMsg* dest, uint8 sensorMask);
BrainMsg* unpackBrainMsg(char *buf);



// You should NOT be using this struct outside of brain_rover.c . I'll move it somewhere safer later
//    when things have settled.
typedef struct {
    uint8 flags;
    uint8 sensorID;
	// Note: This is not always the number of elements in the payload. If a sensor
	//    has samples that are more than 1 byte each you'll have to divide this number
	//    by the length. Note: 
    uint8 payloadLen;
	char payload[MAXI2CBUF - 3];
} RoverMsg;
#define ROVERMSG_MEMBERS 3
// THE FOLLOWING ARE FLAGS FOR RoverMsg.flags
#define SENSOR_RESP 1
// END FLAGS


// HEY DAVE, this is YOURS!
/*
void sendADdata(sensorADData*data, int len) {
	char outBuff[100]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
	packADData(len, data, outBuff, sizeof(outBuff));
	
	startI2Creply(outBuff);
}*/















#endif