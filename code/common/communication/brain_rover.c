
#include "common.h"
#include "brain_rover.h"
#include "debug.h"

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
static int packReturnData(char* data, int payloadLen, RoverMsg* msg, int maxout, uint8 flags, uint8 parameters, uint8 msgid) {
    if (payloadLen + HEADER_MEMBERS > maxout) {
        return 0;
    }
    msg->checksum = 0;

    msg->flags = flags;
    msg->parameters = parameters;
    msg->payloadLen = payloadLen;
    msg->messageid = msgid;

    char* dest = msg->payload;
    char* end = dest + payloadLen;
    msg->checksum = flags+ parameters + payloadLen + msg->messageid;
    while (dest != end) {
        msg->checksum += *data;
        *dest++ = *data++;
    }
    return payloadLen + HEADER_MEMBERS;
}

//returns number of bytes packed
static uint8 packAck(uint8 flags, uint8 parameters, Msg* msg, uint8 outlen, uint8 msgid){ //wifly = 1 if wifly comm, 0 if i2c (used for messageid)
    if(outlen < 5) return 0;
    msg->flags = flags | ACK_FLAG;
    msg->parameters = parameters;
    msg->payloadLen = 0; //0 byte payload for ack
    msg->messageid = msgid;
    msg->checksum = msg->flags + parameters + msg->messageid;
    return HEADER_MEMBERS;
}


//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStartForwardAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x00, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStartBackwardAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x01, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packStopAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x02, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packTurnCWAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x03, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packTurnCCWAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x04, (Msg*) out, outlen, msgid);
}

uint8 packReadjustCWAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x06, (Msg*) out, outlen, msgid);
}

uint8 packReadjustCCWAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x07, (Msg*) out, outlen, msgid);
}

uint8 packGoForwardDistanceTurnAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(MOTOR_COMMANDS, 0x08, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
uint8 packStartFramesAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x00, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
//wifly determines the interface this message passes through
uint8 packFrameDataAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x01, (Msg*) out, outlen, msgid);
}

//Out must always be (at least) 5 bytes
uint8 packStopFramesAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x03, (Msg*) out, outlen, msgid);
}

uint8 packColorSensedAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x04, (Msg*) out, outlen, msgid);
}

uint8 packTurningCompleteAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x05, (Msg*) out, outlen, msgid);
}

uint8 packDoVictoryDanceAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(HIGH_LEVEL_COMMANDS, 0x06, (Msg*) out, outlen, msgid);
}




uint8 packPICDetectErrorAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(ERROR_FLAG, 0x01, (Msg*) out, outlen, msgid);
}

uint8 packSensorErrorAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(ERROR_FLAG, 0x02, (Msg*) out, outlen, msgid);
}

uint8 packWheelErrorAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(ERROR_FLAG, 0x03, (Msg*) out, outlen, msgid);
}

uint8 packChecksumErrorAck(char* out, uint8 outlen, uint8 msgid){
    return packAck(ERROR_FLAG, 0x04, (Msg*) out, outlen, msgid);
}



// Usually called on a PIC
// Returns the number of bytes that have been used of out, or 0 to
//    signify that the buffer was too small (this should never happen, but
//    during development we should be carely to make sure this doesn't happen)
//int packADData(sensorADData* data, int len, char* out, int maxout) {
//	return packReturnData((char*)data, len*sizeof(sensorADData), (RoverMsg*)out, maxout, SENSOR_COMMANDS, sensorADid);
//}

int packEncoderData(char* data, uint8 len, char* out, uint8 maxout, uint8 msgid){
    return packReturnData(data, len*sizeof(encoderData), (RoverMsg*) out, maxout, MOTOR_COMMANDS, encoderID, msgid);
}

int packSensorFrame(char* data, uint8 len, char* out, uint8 maxout, uint8 msgid){
    return packReturnData(data, len*sizeof(sensorFrameData), (RoverMsg*) out, maxout, SENSOR_COMMANDS, sensorFrameID, msgid);
}

int packFrameData(char* data, uint8 len, char* out, uint8 maxout){
    return packReturnData(data, len, (RoverMsg*) out, maxout, HIGH_LEVEL_COMMANDS, 0x01, wifly_messageid++);
}

int packReadFrame(char* data, uint8 len, char* out, uint8 maxout, uint8 msgid){
    return packReturnData(data, len, (RoverMsg*) out, maxout, HIGH_LEVEL_COMMANDS, 0x02, msgid);
}


static uint8 generateError(Msg* errorbuf, uint8 buflen, uint8 parameters, uint8 wifly){
    if(buflen < 5) return 0; //must be at least 5 - can have frames too
    errorbuf->flags = ERROR_FLAG;
    errorbuf->parameters = parameters;
    if(wifly)
        errorbuf->messageid = wifly_messageid++;
    else
        errorbuf->messageid = i2c_messageid++;
    errorbuf->checksum = ERROR_FLAG + parameters + errorbuf->messageid + errorbuf->payloadLen;
	int i;
    for(i = 0; i < errorbuf->payloadLen; i++){
        errorbuf->checksum += errorbuf->payload[i]; //add frames if needed
    }
    return HEADER_MEMBERS + errorbuf->payloadLen;
}

uint8 generateLeftWheelError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x0a, wifly);
}

uint8 generateRightWheelError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x0b, wifly);
}

uint8 generateChecksumError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x0c, wifly);
}

uint8 generateUltrasonicError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x04, wifly);
}

uint8 generateIR1Error(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x05, wifly);
}

uint8 generateIR2Error(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x06, wifly);
}

uint8 generateColorSensorError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x07, wifly);
}

uint8 generateLeftEncoderError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x08, wifly);
}

uint8 generateRightEncoderError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x9, wifly);
}

uint8 generateSensorPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x01, wifly);
}

uint8 generateMotorPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x02, wifly);
}

uint8 generateMasterPICDetectionError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*)errorbuf, buflen, 0x03, wifly);
}

uint8 generateUnknownCommandError(char* errorbuf, uint8 buflen, uint8 wifly){
    return generateError((Msg*) errorbuf, buflen, 0x0d, wifly);
}

uint8 generateErrorFromParam(char* errorbuf, uint8 buflen, uint8 param, uint8 wifly){
    return generateError((Msg*) errorbuf, buflen, param, wifly);
}


uint8 repackBrainMsg(BrainMsg* brainmsg, char* payload, char* outbuf, uint8 buflen, uint8 wifly){
    if(buflen < brainmsg->payloadLen + HEADER_MEMBERS) return 0;
    BrainMsg* msg = (BrainMsg*) outbuf;
    msg->flags = brainmsg->flags;
    msg->parameters = brainmsg->parameters;
    msg->payloadLen = brainmsg->payloadLen;
    if(wifly){
        msg->messageid = wifly_messageid++;
    }
    else{
        msg->messageid = i2c_messageid++;
    }
    msg->checksum = msg->flags + msg->parameters + msg->messageid + msg->payloadLen;
    int i;
    for(i = 0; i < brainmsg->payloadLen; i++){
        msg->checksum += payload[i];
        msg->payload[i] = payload[i];
    }
    return HEADER_MEMBERS + msg->payloadLen;
}

uint8 generateGetSensorFrame(char* out, uint8 buflen){
    if(buflen < 5) return 0;
    Msg* brainmsg = (Msg*) out;
    brainmsg->flags = SENSOR_COMMANDS;
    brainmsg->parameters = 0x01;
    brainmsg->payloadLen = 0; 
    brainmsg->messageid = i2c_messageid++; //will always hit the Master Pic and then move through i2c
    brainmsg->checksum = SENSOR_COMMANDS + 0x01 + brainmsg->messageid + brainmsg->payloadLen;
    return HEADER_MEMBERS;
}

uint8 generateGetEncoderData(char* out, uint8 buflen){
    if(buflen < 5) return 0;
    Msg* brainmsg = (Msg*) out;
    brainmsg->flags = MOTOR_COMMANDS;
    brainmsg->parameters = 0x05;
    brainmsg->payloadLen = 0;
    brainmsg->messageid = i2c_messageid++; //will always hit the Master Pic and then move through i2c
    brainmsg->checksum = MOTOR_COMMANDS + 0x05 + brainmsg->messageid + brainmsg->payloadLen;
    return HEADER_MEMBERS;
}

//payload will never be 0 unless it is not being used.
//turn degrees will be some integer and speed will never be 0
static uint8 generateMotorCommand(char* out, uint8 buflen, uint8 wifly, uint8 parameters, uint8 payload){
    if(payload == 0) {
        if (buflen < HEADER_MEMBERS) return 0;
    }
    else{
        if(buflen < HEADER_MEMBERS + 1) return 0;
    }
    Msg* brainmsg = (Msg*) out;
    brainmsg->flags = MOTOR_COMMANDS;
    brainmsg->parameters = parameters;
    brainmsg->payload[0] = payload;
    if(payload == 0) {
        brainmsg->payloadLen = 0;
    }
    else{
        brainmsg->payloadLen = 1;
    }
    if(wifly)
        brainmsg->messageid = wifly_messageid++;
    else
        brainmsg->messageid = i2c_messageid++;
    brainmsg->checksum = MOTOR_COMMANDS + parameters + brainmsg->messageid + brainmsg->payloadLen + payload;

    if(payload == 0){
        return HEADER_MEMBERS;
    }
    return HEADER_MEMBERS + 1; //+1 for payload
}

uint8 generateStartForward(char* out, uint8 buflen, uint8 wifly, uint8 speed){
    return generateMotorCommand(out, buflen, wifly, 0x00, speed);
}

uint8 generateStartBackward(char* out, uint8 buflen, uint8 wifly, uint8 speed){
    return generateMotorCommand(out, buflen, wifly, 0x01, speed);
}

uint8 generateStop(char* out, uint8 buflen, uint8 wifly){
    return generateMotorCommand(out, buflen, wifly, 0x02, 0);
}

uint8 generateTurnCW(char* out, uint8 buflen, uint8 wifly, uint8 degrees){
    return generateMotorCommand(out, buflen, wifly, 0x03, degrees);
}

uint8 generateTurnCCW(char* out, uint8 buflen, uint8 wifly, uint8 degrees){
    return generateMotorCommand(out, buflen, wifly, 0x04, degrees);
}

uint8 generateReadjustCW(char* out, uint8 buflen, uint8 wifly){
    return generateMotorCommand(out, buflen, wifly, 0x06, 0);
}

uint8 generateReadjustCCW(char* out, uint8 buflen, uint8 wifly){
    return generateMotorCommand(out, buflen, wifly, 0x07, 0);
}

//This command is a special one.  We will handle it in the function
//instead of calling the general motoc command function
uint8 generateGoForwardDistanceTurn(char* out, uint8 buflen, uint8 wifly,
        uint8 speed, uint8 distance, uint8 direction){
    if (buflen < HEADER_MEMBERS + 3) return 0;
    Msg* brainmsg = (Msg*) out;
    brainmsg->flags = MOTOR_COMMANDS;
    brainmsg->parameters = 0x08;
    brainmsg->payload[0] = speed;
    brainmsg->payload[1] = distance;
    brainmsg->payload[2] = direction;
    brainmsg->payloadLen = 3;
    if(wifly)
        brainmsg->messageid = wifly_messageid++;
    else
        brainmsg->messageid = i2c_messageid++;
    brainmsg->checksum = MOTOR_COMMANDS + 0x08 + brainmsg->messageid + brainmsg->payloadLen + speed + distance + direction;

    return HEADER_MEMBERS + 3;
}


//payload will never be 0
//framedata is controlled by a separate function packFrameMessage
static uint8 generateHighLevelCommand(char* out, uint8 buflen, uint8 wifly, uint8 parameters){
    if (buflen < HEADER_MEMBERS) return 0;
    Msg* brainmsg = (Msg*) out;
    brainmsg->flags = HIGH_LEVEL_COMMANDS;
    brainmsg->parameters = parameters;
    brainmsg->payloadLen = 0;
    if(wifly)
        brainmsg->messageid = wifly_messageid++;
    else
        brainmsg->messageid = i2c_messageid++;
    brainmsg->checksum = HIGH_LEVEL_COMMANDS + parameters + brainmsg->messageid + brainmsg->payloadLen;

    return HEADER_MEMBERS;
}

uint8 generateStartFrames(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x00);
}

uint8 generateStopFrames(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x03);
}

uint8 generateReadFrames(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x02);
}

uint8 generateTurnCompleteReq(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x05);
}

uint8 generateTurnCompleteAck(char* out, uint8 buflen, uint8 msgid){
    char payload[1] = {1};
    return packReturnData(payload, 1, (RoverMsg*) out, buflen, HIGH_LEVEL_COMMANDS, 0x05, msgid);
}

uint8 generateTurnCompleteNack(char* out, uint8 buflen, uint8 msgid){
    char payload[1] = {0};
    return packReturnData(payload, 1, (RoverMsg*) out, buflen, HIGH_LEVEL_COMMANDS, 0x05, msgid);
}

uint8 generateColorSensorSensed(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x04);
}

uint8 generateDoVictoryDance(char* out, uint8 buflen, uint8 wifly){
    return generateHighLevelCommand(out, buflen, wifly, 0x06);
}

BrainMsg* unpackBrainMsg(char *buf){
    return (BrainMsg*) buf;
}

void makeHighPriority(char* buf){
    if(isHighPriority(buf))
        return;
    Msg* m = (Msg*)buf;
    m->flags  |= HIGH_PRIORITY;
    m->checksum += HIGH_PRIORITY;
}
uint8 isHighPriority(char* buf){
    Msg* m = (Msg*) buf;
    return (m->flags & HIGH_PRIORITY) == HIGH_PRIORITY;
}

void clearHighPriority(char* buf){
    if(!isHighPriority(buf))
        return;
    Msg* m = (Msg*) buf;
    m->flags &= ~HIGH_PRIORITY; //mask flags with 0x7f
    m->checksum -= HIGH_PRIORITY;
}


void flagInvalidData(char* buf){
    if(isInvalidData(buf))
        return;
    Msg* m = (Msg*) buf;
    m->flags |= FRAME_NOT_VALID;
    m->checksum += FRAME_NOT_VALID;
}
uint8 isInvalidData(char* buf){
    Msg* m = (Msg*) buf;
    return (m->flags & FRAME_NOT_VALID) == FRAME_NOT_VALID;
}

void flagColorSensed(char* buf){
    if(isColorSensed(buf))
        return;
    Msg* m = (Msg*) buf;
    m->flags |= COLOR_SENSED;
    m->checksum += COLOR_SENSED;
}
uint8 isColorSensed(char* buf){
    Msg* m = (Msg*) buf;
    return (m->flags & COLOR_SENSED) == COLOR_SENSED;
}

uint8 isMovementCommand(char* buf){
    Msg* m = (Msg*) buf;
    return (m->flags == MOTOR_COMMANDS) && (m->parameters != 0x05);
}

uint8 isHighLevelCommand(char* buf) {
    Msg* m = (Msg*) buf;
    return m->flags == HIGH_LEVEL_COMMANDS;
}

