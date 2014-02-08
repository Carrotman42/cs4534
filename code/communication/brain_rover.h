
#ifndef BRAIN_ROVER_H_INC
#define BRAIN_ROVER_H_INC


typedef char uint8;
typedef short uint16;


typedef struct {
	uint8 flags;
	uint8 sensorMask;
	uint16 len;
} BrainToRover;


typedef struct {
    uint8 flags;
    uint8 sensorId;
    uint8 count;
	uint8 payload[0];
} RoverMsg;

typedef struct {
	uint8 data;
} sensorADData;

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
// HEY DAVE, this is YOURS!
/*
void sendADdata(sensorADData*data, int len) {
	char outBuff[100]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
	packADData(len, data, outBuff, sizeof(outBuff));
	
	startI2Creply(outBuff);
}*/
void Test();















#endif