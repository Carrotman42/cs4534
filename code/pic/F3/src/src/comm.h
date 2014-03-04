#include "maindefs.h"

#ifndef COMM_H
#define	COMM_H

#include "../../../../common/common.h"
#include "../../../../common/communication/brain_rover.h"
#include "messages.h"
#include "my_i2c.h"
#include "my_uart.h"
#include "motorcomm.h"
#include "sensorcomm.h"

void setBrainData(char* msg, uint8 uart);
void sendResponse();
void sendData(char* outbuf, uint8 buflen, uint8 wifly);

#endif	/* COMM_H */
