#ifndef __vtI2Ch
#define __vtI2Ch
/* include files. */
#include "lpc17xx_i2c.h"
#include "vtUtilities.h"
#include "FreeRTOS.h"
#include "projDefs.h"
#include "semphr.h"

// return codes for vtI2CInit()
#define vtI2CErrInit -1
#define vtI2CInitSuccess 0

// The maximum length of a message to be sent/received over I2C 
#define vtI2CMLen 64

// Structure that is used to define the operate of an I2C peripheral using the vtI2C routines
//   It should be initialized by vtI2CInit() and then not changed by anything... ever
//   A user of the API should never change or access it, it should only pass it as a parameter
typedef struct __vtI2CStruct {
	uint8_t devNum;	  						// Number of the I2C peripheral (0,1,2 on the 1768)
	LPC_I2C_TypeDef *devAddr;	 			// Memory address of the I2C peripheral
	unsigned portBASE_TYPE taskPriority;   	// Priority of the I2C task
	xSemaphoreHandle binSemaphore;		   	// Semaphore used between I2C task and I2C interrupt handler
	xQueueHandle inQ;					   	// Queue used to send messages from other tasks to the I2C task
	xQueueHandle outQ;						// Queue used by the I2C task to send out results
} vtI2CStruct;

/* ********************************************************************* */
// The following are the public API calls that other tasks should use to work with the I2C task

// Args:
//   dev: pointer to the vtI2CStruct data structure
//   i2cDevNum: The number of the i2c device -- 0, 1, or 2
//   taskPriority: At what priority should this task be run?
//   i2cSpeed: Clock speed of the i2c bus
// Return:
//   if successful, returns vtI2CInitSuccess
//   if not, should return vtI2CErrInit
// Must be called for each I2C device initialized (0, 1, or 2) and used
int vtI2CInit(vtI2CStruct *devPtr,uint8_t i2cDevNum,unsigned portBASE_TYPE taskPriority,uint32_t i2cSpeed);

// A simple routine to use for filling out and sending a message to the I2C thread
//   You may want to make your own versions of these as they are not suited to all purposes
// Args
//   dev: pointer to the vtI2CStruct data structure
//   msgType: The message type value -- does not get sent on the wire, but is included in the response in the message queue
//   slvAddr: The address of the i2c slave device you are addressing
//   txLen: The number of bytes you want to send
//   txBuf: The buffer holding the bytes you want to send
//   rxLen: The number of bytes that you would like to receive
// Return:
//   Result of the call to xQueueSend()
portBASE_TYPE vtI2CEnQ(vtI2CStruct *dev,uint8_t msgType,uint8_t slvAddr,uint8_t txLen,const uint8_t *txBuf,uint8_t rxLen);

// A simple routine to use for retrieving a message from the I2C thread
// Args
//   dev: pointer to the vtI2CStruct data structure
//   maxRxLen: The maximum number of bytes that your receive buffer can hold
//   rxBuf: The buffer that you are providing into which the message will be copied
//   rxLen: The number of bytes that were actually received
//   msgType: The message type value -- does not get sent/received on the wire, but is included in the response in the message queue
//   status: Return code of the operation (you will need to dive into the code to understand the status values)
// Return:
//   Result of the call to xQueueReceive()
portBASE_TYPE vtI2CDeQ(vtI2CStruct *dev,uint8_t maxRxLen,uint8_t *rxBuf,uint8_t *rxLen,uint8_t *msgType,uint8_t *status);
#endif
