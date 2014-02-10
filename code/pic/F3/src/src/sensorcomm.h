#include "../../../../common/common.h"


#ifndef SENSORCOMM_H
#define	SENSORCOMM_H

#include "../../../../common/communication/brain_rover.h"

typedef struct{
    uint8 len;
    sensorADData data[MSGLEN];
} sensorADaccumulator;

void setBrainReqData(char* msg);
void sendRequestedData();
void sendADdata();
void addDataPoints(int sensorid, void* data, int len);
void addADDataPoints(sensorADData* data, int len);
void resetADacc();
void resetAccumulators();

#endif	/* SENSORCOMM_H */
