
#include "../common.h"
#include "brain_rover.h"
#include "../../pic/F3/src/src/debug.h"

static uint8 i2c_messageid = 0;
static uint8 wifly_messageid = 0;

// Use this method instead of accessing BrainMsg directly
//   because if the protocol changes we want to just edit this
//   func in this file, rather than looking everywhere around the
//   project and changing it everywhere.
void packBrainMsgRequest(BrainMsg* dest, uint8 sensorMask) {
	dest->flags = SENSOR_COMMANDS;
	dest->parameters = sensorMask;
	dest->checksum = SENSOR_COMMANDS + sensorMask;
	dest->messageid = 0;
	dest->payloadLen = 0;
}

// Used in this file only to generically make a RoverMsg. each "pack[SENSOR]Data" should call this one
//    just in case we change the format of RoverMsg
static int packReturnData(char* data, int payloadLen, RoverMsg* msg, int maxout, int sensorID) {
    if (payloadLen + HEADER_MEMBERS >= maxout) {
        return 0;
    }

    msg->flags = SENSOR_COMMANDS;
    msg->parameters = sensorID;
    msg->payloadLen = payloadLen;

    char* dest = msg->payload;
    char* end = dest + payloadLen;
    while (dest != end) {
        *dest++ = *data++;
    }
    return payloadLen + HEADER_MEMBERS;
}

static void packAck(uint8 flags, uint8 parameters, Msg* msg, uint8 wifly){ //wifly = 1 if wifly comm, 0 if i2c (used for messageid)
    msg->flags = flags;
    msg->parameters = parameters;
    msg->payloadLen = 0; //0 byte payload for ack
    if(wifly)
        msg->messageid = wifly_messageid++;
    else
        msg->messageid = i2c_messageid++;
    msg->checksum = flags + parameters + msg->messageid;
}


//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStartForwardAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5) return -1;
    packAck(MOTOR_COMMANDS, 0x00, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStartBackwardAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5) return -1;
    packAck(MOTOR_COMMANDS, 0x01, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStopAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5) return -1;
    packAck(MOTOR_COMMANDS, 0x02, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packTurnCWAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(MOTOR_COMMANDS, 0x03, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packTurnCCWAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(MOTOR_COMMANDS, 0x04, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
uint8 packStartFramesAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(HIGH_LEVEL_COMMANDS, 0x00, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packFrameDataAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(HIGH_LEVEL_COMMANDS, 0x01, (Msg*) out, wifly);
    return 0;
}

//Out must always be (at least) 5 bytes
uint8 packStopFramesAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(HIGH_LEVEL_COMMANDS, 0x03, (Msg*) out, wifly);
    return 0;
}

uint8 packPICDetectErrorAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(ERROR_FLAG, 0x01, (Msg*) out, wifly);
    return 0;
}

uint8 packSensorErrorAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(ERROR_FLAG, 0x02, (Msg*) out, wifly);
    return 0;
}

uint8 packWheelErrorAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(ERROR_FLAG, 0x03, (Msg*) out, wifly);
    return 0;
}

uint8 packChecksumErrorAck(char* out, uint8 outlen, uint8 wifly){
    if(outlen < 5)  return -1;
    packAck(ERROR_FLAG, 0x04, (Msg*) out, wifly);
    return 0;
}


// Usually called on a PIC
// Returns the number of bytes that have been used of out, or 0 to
//    signify that the buffer was too small (this should never happen, but
//    during development we should be carely to make sure this doesn't happen)
int packADData(sensorADData* data, int len, char* out, int maxout) {
	return packReturnData((char*)data, len*sizeof(sensorADData), (RoverMsg*)out, maxout, sensorADid);
}

BrainMsg* unpackBrainMsg(char *buf){
    return (BrainMsg*) buf;
}

// Usually called on the ARM. Will move the data along to the next method for processing
int unpackRoverMsg(char* in, int len, RoverMsgRouter* handler) {
	RoverMsg* msg = (RoverMsg*)in;
	len -= sizeof(RoverMsg);
	// Validate the internal structure of the packet
	//if (len != msg->payloadLen) return -3;
	
	len = msg->payloadLen;
	/*char buf[10];
	buf[0] = '0' + len;
	buf[1] = '0' + msg->sensorID;
	buf[2] = 0;
	LCDwriteLn(2, buf);*/
	if (msg->flags & SENSOR_COMMANDS) {
		switch (msg->parameters) {
			case sensorADid:
				return handler->adFunc((sensorADData*)(msg->payload), len/sizeof(sensorADData));
			default:
				// Unknown sensor
				return -2;
		}
	} else {
		// Unknown msg
		return -1;
	}
}
