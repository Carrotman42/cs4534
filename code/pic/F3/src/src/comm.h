#include "maindefs.h"

#ifndef COMM_H
#define	COMM_H

#include "../../../../common/common.h"
#include "../../../../common/sensor_types.h"
#include "../../../../common/communication/brain_rover.h"
#include "messages.h"
#include "my_i2c.h"
#include "my_uart.h"
#include "motorcomm.h"
#include "sensorcomm.h"

void setBrainData(char* msg, uint8 uart);
uint8 sendResponse();
void sendData(char* outbuf, uint8 buflen, uint8 wifly);
void handleCommand();
uint8 sendFrames();

#define UART_COMM 1
#define I2C_COMM 0

#endif	/* COMM_H */
