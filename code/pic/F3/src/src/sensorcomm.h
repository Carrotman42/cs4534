#include "maindefs.h"

#ifndef SENSORCOMM_H
#define	SENSORCOMM_H

#include "common.h"
#include "brain_rover.h"
#include "messages.h"
#include "my_i2c.h"

/*typedef struct{
    uint8 len;
    sensorADData data[MAX_I2C_SENSOR_DATA_LEN];
} sensorADaccumulator;*/

/*void setBrainReqData(char* msg);
void sendRequestedData();
void sendADdata();
void addDataPoints(int sensorid, void* data, int len);
void addADDataPoints(sensorADData* data, int len);
void resetADacc();
void resetAccumulators();*/

#ifdef SENSOR_PIC
void sendSensorFrame(uint8 msgid);

#elif defined(PICMAN) || defined(ARM_EMU)
void colorSensorTriggered();
uint8 isColorSensorTriggered();
void clearColorSensorStatus();
uint8 timesColorSensorTriggered();
#endif

#endif	/* SENSORCOMM_H */
