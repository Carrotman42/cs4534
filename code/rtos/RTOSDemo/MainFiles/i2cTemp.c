#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "semphr.h"

/* include files. */
#include "vtUtilities.h"
#include "vtI2C.h"
#include "i2cTemp.h"
#include "I2CTaskMsgTypes.h"
#include "armcommon.h"
#include "klcd.h"

/* *********************************************** */
// definitions and data structures that are private to this file
// Length of the queue to this task
#define vtTempQLen 10 
// actual data structure that is sent in a message
typedef struct __vtTempMsg {
	uint8_t msgType;
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[vtTempMaxLen+1]; // On the way in, message to be sent, on the way out, message received (if any)
} vtTempMsg;

// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the i2c operations	-- almost certainly too large, see LCDTask.c for details on how to check the size
#define baseStack 3
#if PRINTF_VERSION == 1
#define i2cSTACK_SIZE		((baseStack+5)*configMINIMAL_STACK_SIZE)
#else
#define i2cSTACK_SIZE		(baseStack*configMINIMAL_STACK_SIZE)
#endif

// end of defs
/* *********************************************** */

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( vi2cTempUpdateTask, pvParameters );

/*-----------------------------------------------------------*/
// Public API
void vStarti2cTempTaskp(vtTempStruct *params,unsigned portBASE_TYPE uxPriority, vtI2CStruct *i2c)
{
	// Create the queue that will be used to talk to this task
	if ((params->inQ = xQueueCreate(vtTempQLen,sizeof(vtTempMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	/* Start the task */
	portBASE_TYPE retval;
	params->dev = i2c;
	if ((retval = xTaskCreate( vi2cTempUpdateTask, ( signed char * ) "i2cTemp", i2cSTACK_SIZE, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

portBASE_TYPE SendTempTimerMsga(vtTempStruct *tempData,portTickType ticksElapsed,portTickType ticksToBlock)
{
	if (tempData == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	vtTempMsg tempBuffer;
	tempBuffer.length = sizeof(ticksElapsed);
	if (tempBuffer.length > vtTempMaxLen) {
		// no room for this message
		VT_HANDLE_FATAL_ERROR(tempBuffer.length);
	}
	memcpy(tempBuffer.buf,(char *)&ticksElapsed,sizeof(ticksElapsed));
	tempBuffer.msgType = TempMsgTypeTimer;
	return(xQueueSend(tempData->inQ,(void *) (&tempBuffer),ticksToBlock));
}

portBASE_TYPE SendTempValueMsg(vtTempStruct *tempData,uint8_t msgType,uint8_t value,portTickType ticksToBlock)
{
	vtTempMsg tempBuffer;

	if (tempData == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	tempBuffer.length = sizeof(value);
	if (tempBuffer.length > vtTempMaxLen) {
		// no room for this message
		VT_HANDLE_FATAL_ERROR(tempBuffer.length);
	}
	memcpy(tempBuffer.buf,(char *)&value,sizeof(value));
	tempBuffer.msgType = msgType;
	return(xQueueSend(tempData->inQ,(void *) (&tempBuffer),ticksToBlock));
}

// End of Public API
/*-----------------------------------------------------------*/
int getMsgType(vtTempMsg *Buffer)
{
	return(Buffer->msgType);
}
uint8_t getValue(vtTempMsg *Buffer)
{
	uint8_t *ptr = (uint8_t *) Buffer->buf;
	return(*ptr);
}

// I2C commands for the temperature sensor
	const uint8_t i2cCmdInit[]= {0xAC,0x00};
	const uint8_t i2cCmdStartConvert[]= {0xEE};
	const uint8_t i2cCmdStopConvert[]= {0x22};
	const uint8_t i2cCmdReadVals[]= {0xAA};
	const uint8_t i2cCmdReadCnt[]= {0xA8};
	const uint8_t i2cCmdReadSlope[]= {0xA9};
// end of I2C command definitions

// Definitions of the states for the FSM below
const uint8_t fsmStateInit1Sent = 0;
const uint8_t fsmStateInit2Sent = 1;
const uint8_t fsmStateTempRead1 = 2;
const uint8_t fsmStateTempRead2 = 3;
const uint8_t fsmStateTempRead3 = 4;
// This is the actual task that is run
static portTASK_FUNCTION( vi2cTempUpdateTask, pvParameters )
{
	float temperature = 0.0;
	float countPerC = 100.0, countRemain=0.0;
	// Get the parameters
	vtTempStruct *param = (vtTempStruct *) pvParameters;
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->dev;
	// Buffer for receiving messages
	vtTempMsg msgBuffer;
	uint8_t currentState;

	// Assumes that the I2C device (and thread) have already been initialized

	// This task is implemented as a Finite State Machine.  The incoming messages are examined to see
	//   whether or not the state should change.
	//
	// Temperature sensor configuration sequence (DS1621) Address 0x4F
	if (vtI2CEnQ(devPtr,vtI2CMsgTypeTempInit,0x4F,sizeof(i2cCmdInit),i2cCmdInit,0) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	currentState = fsmStateInit1Sent;
	// Like all good tasks, this should never exit
	for(;;)
	{
		// Wait for a message from either a timer or from an I2C operation
		if (xQueueReceive(param->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}

		int msgT = getMsgType(&msgBuffer);
		
		if (currentState < 4) {
			//DBGval(currentState);
		}
		// Now, based on the type of the message and the state, we decide on the new state and action to take
		switch(msgT) {
		case vtI2CMsgTypeTempInit: {
			if (currentState == fsmStateInit1Sent) {
				currentState = fsmStateInit2Sent;
				// Must wait 10ms after writing to the temperature sensor's configuration registers(per sensor data sheet)
				vTaskDelay(10/portTICK_RATE_MS);
				// Tell it to start converting
				if (vtI2CEnQ(devPtr,vtI2CMsgTypeTempInit,0x4F,sizeof(i2cCmdStartConvert),i2cCmdStartConvert,0) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
			} else 	if (currentState == fsmStateInit2Sent) {
				currentState = fsmStateTempRead1;
			} else {
				// unexpectedly received this message
				VT_HANDLE_FATAL_ERROR(0);
			}
			break;
		}
		case TempMsgTypeTimer: {
			// Timer messages never change the state, they just cause an action (or not) 
			if ((currentState != fsmStateInit1Sent) && (currentState != fsmStateInit2Sent)) {
				// Read in the values from the temperature sensor
				// We have three transactions on i2c to read the full temperature 
				//   we send all three requests to the I2C thread (via a Queue) -- responses come back through the conductor thread
				// Temperature read -- use a convenient routine defined above
				if (vtI2CEnQ(devPtr,vtI2CMsgTypeTempRead1,0x4F,sizeof(i2cCmdReadVals),i2cCmdReadVals,2) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
				// Read in the read counter
				if (vtI2CEnQ(devPtr,vtI2CMsgTypeTempRead2,0x4F,sizeof(i2cCmdReadCnt),i2cCmdReadCnt,1) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
				// Read in the slope;
				if (vtI2CEnQ(devPtr,vtI2CMsgTypeTempRead3,0x4F,sizeof(i2cCmdReadSlope),i2cCmdReadSlope,1) != pdTRUE) {
					VT_HANDLE_FATAL_ERROR(0);
				}
			} else {
				// just ignore timer messages until initialization is complete
			} 
			break;
		}
		case vtI2CMsgTypeTempRead1: {
			if (currentState == fsmStateTempRead1) {
				currentState = fsmStateTempRead2;
				temperature = getValue(&msgBuffer);
			} else {
				// unexpectedly received this message
				VT_HANDLE_FATAL_ERROR(0);
			}
			break;
		}
		case vtI2CMsgTypeTempRead2: {
			if (currentState == fsmStateTempRead2) {
				currentState = fsmStateTempRead3;
				countRemain = getValue(&msgBuffer);
			} else {
				// unexpectedly received this message
				VT_HANDLE_FATAL_ERROR(0);
			}
			break;
		}
		case vtI2CMsgTypeTempRead3: {
			if (currentState == fsmStateTempRead3) {
				currentState = fsmStateTempRead1;
				countPerC = getValue(&msgBuffer);

				// Now have all of the values, so compute the temperature and send to the LCD Task
				// Do the accurate temperature calculation
				temperature += -0.25 + ((countPerC-countRemain)/countPerC);

				// we do not have full printf (so no %f) and therefore need to print out integers
				//printf("Kevn %d F (%d C)\n",lrint(32.0 + ((9.0/5.0)*temperature)), lrint(temperature));
				TextLCDMsg* buf = LCDgetTextBuffer();
				snprintf(buf->text, CHARS+1, "Temp=%d F (%d C)",(int)lrint(32.0 + ((9.0/5.0)*temperature)),(int)lrint(temperature));

				buf->line = 1;
				LCDcommitBuffer(buf);
			} else {
				// unexpectedly received this message
				VT_HANDLE_FATAL_ERROR(0);
			}
			break;
		}
		default: {
			VT_HANDLE_FATAL_ERROR(getMsgType(&msgBuffer));
			break;
		}
		}


	}
}

