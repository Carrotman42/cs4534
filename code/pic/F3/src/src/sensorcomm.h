

#ifndef SENSORCOMM_H
#define	SENSORCOMM_H

#include "../../../../common/common.h"
#include "../../../../common/communication/brain_rover.h"
#include "messages.h"

typedef struct{
    uint8 len;
    sensorADData data[100];
} sensorADaccumulator;

void setBrainReqData(char* msg);
void sendRequestedData();
void sendADdata();
void addDataPoints(int sensorid, void* data, int len);
void addADDataPoints(sensorADData* data, int len);
void resetADacc();
void resetAccumulators();

#endif	/* SENSORCOMM_H */
