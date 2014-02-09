#include "../../../../common/common.h"

#ifdef PIC

#ifndef SENSORCOMM_H
#define	SENSORCOMM_H

#include "../../../../common/communication/brain_rover.h"

void setBrainReqData(char* msg);
void sendRequestedData();
void sendADdata(sensorADData*data, int len);

#endif	/* SENSORCOMM_H */

#endif //PIC