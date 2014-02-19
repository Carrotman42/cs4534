#include "maindefs.h"
#ifdef SENSOR_PIC

#ifndef SENSORCOMM_H
#define	SENSORCOMM_H

#include "../../../../common/common.h"
#include "../../../../common/communication/brain_rover.h"
#include "messages.h"
#include "my_i2c.h"

typedef struct{
    uint8 len;
    sensorADData data[MAX_I2C_SENSOR_DATA_LEN];
} sensorADaccumulator;

void setBrainReqData(char* msg);
void sendRequestedData();
void sendADdata();
void addDataPoints(int sensorid, void* data, int len);
void addADDataPoints(sensorADData* data, int len);
void resetADacc();
void resetAccumulators();

#endif	/* SENSORCOMM_H */

#endif //SENSOR_PIC