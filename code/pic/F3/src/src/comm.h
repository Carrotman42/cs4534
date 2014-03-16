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

void setBrainDataHP(char* msg);
void setRoverDataHP(char* msg);
void setBrainDataLP(char* msg);
void setRoverDataLP(char* msg);
uint8 sendResponse(BrainMsg* brain, uint8 wifly);
void sendData(char* outbuf, uint8 buflen, uint8 wifly);
void handleMessageHP(uint8 source, uint8 dest);
void handleMessageLP(uint8 source, uint8 dest);
uint8 sendFrames();

#if defined(MASTER_PIC) || defined(PICMAN)
static void propogateCommand(BrainMsg* brain, char* payload, uint8 addr, uint8 dest);
void sendHighLevelAckResponse(uint8 parameters, uint8 messageid, uint8 wifly);
void handleRoverDataHP();
void handleRoverDataLP();
#endif


#define UART_COMM 1
#define I2C_COMM 0

#endif	/* COMM_H */
