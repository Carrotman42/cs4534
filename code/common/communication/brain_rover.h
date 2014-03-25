
#ifndef BRAIN_ROVER_H_INC
#define BRAIN_ROVER_H_INC

#include "sensor_types.h"

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
#define ACK_FLAG 0x20
#define COLOR_SENSED 0x10
#define FRAME_NOT_VALID 0x08
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


uint8 packStartForwardAck(char* out, uint8 outlen, uint8 msgid);
uint8 packStartBackwardAck(char* out, uint8 outlen, uint8 msgid);
uint8 packStopAck(char* out, uint8 outlen, uint8 msgid);
uint8 packTurnCWAck(char* out, uint8 outlen, uint8 msgid);
uint8 packTurnCCWAck(char* out, uint8 outlen, uint8 msgid);
uint8 packStartFramesAck(char* out, uint8 outlen, uint8 msgid);
uint8 packFrameDataAck(char* out, uint8 outlen, uint8 msgid);
uint8 packStopFramesAck(char* out, uint8 outlen, uint8 msgid);
uint8 packColorSensedAck(char* out, uint8 outlen, uint8 msgid);
uint8 packTurningCompleteAck(char* out, uint8 outlen, uint8 msgid);

uint8 packPICDetectErrorAck(char* out, uint8 outlen, uint8 msgid);
uint8 packSensorErrorAck(char* out, uint8 outlen, uint8 msgid);
uint8 packWheelErrorAck(char* out, uint8 outlen, uint8 msgid);
uint8 packChecksumErrorAck(char* out, uint8 outlen, uint8 msgid);

uint8 generateLeftWheelError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateRightWheelError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateChecksumError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateUltrasonicError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateIR1Error(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateIR2Error(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateColorSensorError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateLeftEncoderError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateRightEncoderError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateSensorPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateMotorPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateMasterPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly);
uint8 generateUnknownCommandError(char* errorbuf, uint8 buflen, uint8 wifly);

uint8 repackBrainMsg(BrainMsg* brainmsg, char* payload, char* outbuf, uint8 buflen, uint8 wifly);
uint8 generateGetSensorFrame(char* out, uint8 buflen); //
uint8 generateGetEncoderData(char* out, uint8 buflen);


uint8 generateStartForward(char* out, uint8 buflen, uint8 wifly, uint8 speed);
uint8 generateStartBackward(char* out, uint8 buflen, uint8 wifly, uint8 speed);
uint8 generateStop(char* out, uint8 buflen, uint8 wifly);
uint8 generateTurnCW(char* out, uint8 buflen, uint8 wifly, uint8 degrees);
uint8 generateTurnCCW(char* out, uint8 buflen, uint8 wifly, uint8 degrees);

uint8 generateStartFrames(char* out, uint8 buflen, uint8 wifly);
uint8 generateStopFrames(char* out, uint8 buflen, uint8 wifly);
uint8 generateReadFrames(char* out, uint8 buflen, uint8 wifly);
uint8 generateTurnCompleteReq(char* out, uint8 buflen, uint8 wifly);
uint8 generateTurnCompleteAck(char* out, uint8 buflen, uint8 msgid);
uint8 generateTurnCompleteNack(char* out, uint8 buflen, uint8 msgid);
uint8 generateColorSensorSensed(char* out, uint8 buflen, uint8 wifly);

int packEncoderData(char* data, uint8 len, char* out, uint8 maxout, uint8 msgid);
int packSensorFrame(char* data, uint8 len, char* out, uint8 maxout, uint8 msgid);
int packFrameData(char* data, uint8 len, char* out, uint8 maxout);
int packReadFrame(char* data, uint8 len, char* out, uint8 maxout);

void makeHighPriority(char* buf);
uint8 isHighPriority(char* buf);
void clearHighPriority(char* buf);











#endif