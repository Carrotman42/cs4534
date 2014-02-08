
#include "common.h"
#include "brain_rover.h"

void Test() {}

/*
void packADData(int len, sensorADData* data, char* out, int maxout) {
	RoverMsg* msg = (RoverMsg*)out;
	msg->flags = RETURN_DATA;
	msg->sensorID = sensorADid;
	msg->count = len;
	memcpy(msg->payload, data, len*sizeof(sensorADData));
}

void unpackADData(char* in, int len, sensorADData* out, int maxout) {
	RoverMsg* msg = (RoverMsg*)out;
	if(msg->flags != RETURN_DATA || msg->sesorID != sensorADid) {
		FAIL(0);
	}
	msg->flags = RETURN_DATA;
	msg->sensorID = sensorADid;
	msg->count = len;
	memcpy(msg->payload, data, len*sizeof(sensorADData));
}
*/