
#ifndef BRAIN_ROVER_H_INC
#define BRAIN_ROVER_H_INC

#include "sensor_types.h"

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

void packADData(int len, sensorADData* data, char* out, int maxout);
void unpackADData(char* in, int len, sensorADData* out, int maxout);

// HEY DAVE, this is YOURS!
/*
void sendADdata(sensorADData*data, int len) {
	char outBuff[100]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
	packADData(len, data, outBuff, sizeof(outBuff));
	
	startI2Creply(outBuff);
}*/
void Test();















#endif