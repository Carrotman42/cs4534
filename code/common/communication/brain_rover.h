
#ifndef BRAIN_ROVER_H_INC
#define BRAIN_ROVER_H_INC

#include "../sensor_types.h"
//#include "../../pic/F3/src/src/my_i2c.h"


typedef struct {
    uint8 flags;
    uint8 parameters;
    uint8 messageid;
    uint8 payloadLen;
	// Note: this should always be zero. It is this way so that every message is the same size to
	//   simplify UART stuff.
    uint8 checksum;
    char payload[];
} Msg;

// You can use this struct because it is constant length
typedef Msg BrainMsg;

typedef struct {
	int (*adFunc)(sensorADData* data, int len);
} RoverMsgRouter;

// THE FOLLOWING ARE FLAGS
#define SENSOR_COMMANDS 0x01
#define MOTOR_COMMANDS 0x02
#define HIGH_LEVEL_COMMANDS 0x04
#define HIGH_PRIORITY 0x80
#define ERROR_FLAG 0x40
// END FLAGS

// Should only use the functions prototyped out here
int packADData(sensorADData* data, int len, char* out, int maxout);
int unpackRoverMsg(char* in, int len, RoverMsgRouter* handler);
void packBrainMsgRequest(BrainMsg* dest, uint8 sensorMask);
BrainMsg* unpackBrainMsg(char *buf);


		  
// You should NOT be using this struct outside of brain_rover.c . I'll move it somewhere safer later
//    when things have settled.
typedef Msg RoverMsg;

#define HEADER_MEMBERS (sizeof(Msg))
//#define offsetof(type, member) (int)(&((type*)0)->member)
#define PAYLOADLEN_POS offsetof(Msg, payloadLen)
#define CHECKSUM_POS offsetof(Msg, checksum)


uint8 packStartForwardAck(char* out, uint8 outlen, uint8 wifly);
uint8 packStartBackwardAck(char* out, uint8 outlen, uint8 wifly);
uint8 packStopAck(char* out, uint8 outlen, uint8 wifly);
uint8 packTurnCWAck(char* out, uint8 outlen, uint8 wifly);
uint8 packTurnCCWAck(char* out, uint8 outlen, uint8 wifly);
uint8 packStartFramesAck(char* out, uint8 outlen, uint8 wifly);
uint8 packFrameDataAck(char* out, uint8 outlen, uint8 wifly);
uint8 packStopFramesAck(char* out, uint8 outlen, uint8 wifly);
uint8 packPICDetectErrorAck(char* out, uint8 outlen, uint8 wifly);
uint8 packSensorErrorAck(char* out, uint8 outlen, uint8 wifly);
uint8 packWheelErrorAck(char* out, uint8 outlen, uint8 wifly);
uint8 packChecksumErrorAck(char* out, uint8 outlen, uint8 wifly);













#endif