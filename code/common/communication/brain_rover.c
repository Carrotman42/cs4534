
#include "../common.h"
#include "brain_rover.h"
#include "../../pic/F3/src/src/debug.h"

// Use this method instead of accessing BrainMsg directly
//   because if the protocol changes we want to just edit this
//   func in this file, rather than looking everywhere around the
//   project and changing it everywhere.
void packBrainMsgRequest(BrainMsg* dest, uint8 sensorMask) {
	dest->flags = SENSOR_REQ;
	dest->sensorMask = sensorMask;
}

// Used in this file only to generically make a RoverMsg. each "pack[SENSOR]Data" should call this one
//    just in case we change the format of RoverMsg
static int packReturnData(char* data, int payloadLen, RoverMsg* msg, int maxout, int sensorID) {
	if (payloadLen + ROVERMSG_MEMBERS >= maxout) {
		return 0;
	}
        
	msg->flags = SENSOR_RESP;
	msg->sensorID = sensorID;
	msg->payloadLen = payloadLen;

	char* dest = msg->payload;
	char* end = dest + payloadLen;
	while (dest != end) {
		*dest++ = *data++;				 		
	}
	return payloadLen + ROVERMSG_MEMBERS;
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
	if (msg->flags & SENSOR_RESP) {
		switch (msg->sensorID) {
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
