#include <stdlib.h>
#include <stdio.h>

#include "vtI2C.h"
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "semphr.h"

/* include files. */
#include "lpc17xx_i2c.h"
#include "vtUtilities.h"

#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_pinsel.h"

/* ************************************************ */
// Private definitions used in the Public API
// Structure used to define the messages that are sent to/from the I2C thread 
typedef struct __vtI2CMsg {
	uint8_t *dest; // Receive buffer
	uint8_t destLen; // Receive buffer max length, or length received on the way out
	uint8_t status;  // Status response for msg
	uint8_t slvAddr; // Address of the device to whom the message is being sent (or was sent)
	
	// Msg to send
	BrainMsg data;
} vtI2CMsg;
// Length of the message queues to/from this task
#define vtI2CQLen 10

#define vtI2CTransferFailed -2
#define vtI2CIntPriority 7

// Here is where we define an array of pointers that lets communication occur between the interrupt handler and the rest of the code in this file
static 	vtI2CStruct *devStaticPtr[3];

// I have set this to a large stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the I2C operations -- it is possible/very likely these are much larger than needed (see LCDtask.c for how to check the stack size)
#define baseStack 3
#if PRINTF_VERSION == 1
#define i2cSTACK_SIZE		((baseStack+5)*configMINIMAL_STACK_SIZE)
#else
#define i2cSTACK_SIZE		(baseStack*configMINIMAL_STACK_SIZE)
#endif

/* The I2C monitor tasks. */
static portTASK_FUNCTION_PROTO( vI2CMonitorTask, pvParameters );
// End of private definitions
/* ************************************************ */

/* ************************************************ */
// Public API Functions
//
// Note: This will startup an I2C thread, once for each call to this routine
int vtI2CInit(vtI2CStruct *devPtr,uint8_t i2cDevNum,unsigned portBASE_TYPE taskPriority,uint32_t i2cSpeed)
{
	PINSEL_CFG_Type PinCfg;

	devPtr->devNum = i2cDevNum;
	devPtr->taskPriority = taskPriority;

	int retval = vtI2CInitSuccess;
	switch (devPtr->devNum) {
		case 0: {
			devStaticPtr[0] = devPtr; // Setup the permanent variable for use by the interrupt handler
			devPtr->devAddr = LPC_I2C0;
			// Start with the interrupts disabled *and* make sure we have the priority correct
			NVIC_SetPriority(I2C0_IRQn,vtI2CIntPriority);	
			NVIC_DisableIRQ(I2C0_IRQn);
			// Init I2C pin connect
			PinCfg.OpenDrain = 0;
			PinCfg.Pinmode = 0;
			PinCfg.Funcnum = 1;
			PinCfg.Pinnum = 27;
			PinCfg.Portnum = 0;
			PINSEL_ConfigPin(&PinCfg);
			PinCfg.Pinnum = 28;
			PINSEL_ConfigPin(&PinCfg);
			break;
		}
		case 1: {
			devStaticPtr[1] = devPtr; // Setup the permanent variable for use by the interrupt handler
			devPtr->devAddr = LPC_I2C1;
			// Start with the interrupts disabled *and* make sure we have the priority correct
			NVIC_SetPriority(I2C1_IRQn,vtI2CIntPriority);	
			NVIC_DisableIRQ(I2C1_IRQn);
			// Init I2C pin connect
			PinCfg.OpenDrain = 0;
			PinCfg.Pinmode = 0;
			PinCfg.Funcnum = 3;
			PinCfg.Pinnum = 0;
			PinCfg.Portnum = 0;
			PINSEL_ConfigPin(&PinCfg);
			PinCfg.Pinnum = 1;
			PINSEL_ConfigPin(&PinCfg);
			break;
		}
		default: {
			return(vtI2CErrInit);
			break;
		}
	}

	// Create semaphore to communicate with interrupt handler
	vSemaphoreCreateBinary(devPtr->binSemaphore);
	if (devPtr->binSemaphore == NULL) {
		return(vtI2CErrInit);
	}
	// Need to do an initial "take" on the semaphore to ensure that it is initially blocked
	if (xSemaphoreTake(devPtr->binSemaphore,0) != pdTRUE) {
		// free up everyone and go home
		vQueueDelete(devPtr->binSemaphore);
		return(vtI2CErrInit);
	}

	// Allocate the two queues to be used to communicate with other tasks
	if ((devPtr->inQ = xQueueCreate(vtI2CQLen,sizeof(vtI2CMsg))) == NULL) {
		// free up everyone and go home
		vQueueDelete(devPtr->binSemaphore);
		return(vtI2CErrInit);
	}
	if ((devPtr->outQ = xQueueCreate(vtI2CQLen,sizeof(vtI2CMsg))) == NULL) {
		// free up everyone and go home
		vQueueDelete(devPtr->binSemaphore);
		vQueueDelete(devPtr->outQ);
		return(vtI2CErrInit);
	}

	// Initialize  I2C peripheral
	I2C_Init(devPtr->devAddr, i2cSpeed);

	// Enable  I2C operation
	I2C_Cmd(devPtr->devAddr, ENABLE);

	/* Start the task */
	char taskLabel[8];
	sprintf(taskLabel,"I2C%d",devPtr->devNum);
	if ((retval = xTaskCreate( vI2CMonitorTask, (signed char*) taskLabel, i2cSTACK_SIZE,(void *) devPtr, devPtr->taskPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
		return(vtI2CErrInit); // return is just to keep the compiler happy, we will never get here
	} else {
		return vtI2CInitSuccess;
	}
}

portBASE_TYPE ki2cReadReq(vtI2CStruct *dev, uint8_t slvAddr, BrainMsg msg, uint8_t* dest, uint8_t destLen, uint8_t* destAct) {
	vtI2CMsg msgBuf;
	msgBuf.dest = dest;
	msgBuf.destLen = destLen;
	msgBuf.slvAddr = slvAddr;
	msgBuf.data = msg;

	SEND(dev->inQ, msgBuf);
	RECV(dev->outQ, msgBuf);
	
	*destAct = msgBuf.destLen;
	return msgBuf.status;
}

#include "kdbg.h"

// End of public API Functions
/* ************************************************ */

void kevinsMasterHandler(LPC_I2C_TypeDef *devAddr);

// i2c interrupt handler
static __INLINE void vtI2CIsr(LPC_I2C_TypeDef *devAddr,xSemaphoreHandle *binSemaphore) {
	kevinsMasterHandler(devAddr);
	if (I2C_MasterTransferComplete(devAddr)) {
		static signed portBASE_TYPE xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(*binSemaphore,&xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}
// Simply pass on the information to the real interrupt handler above (have to do this to work for multiple i2c peripheral units on the LPC1768
void vtI2C0Isr(void) {
	// Log the I2C status code
	vtITMu8(vtITMPortI2C0IntHandler,((devStaticPtr[0]->devAddr)->I2STAT & I2C_STAT_CODE_BITMASK));
	vtI2CIsr(devStaticPtr[0]->devAddr,&(devStaticPtr[0]->binSemaphore));
}

// Simply pass on the information to the real interrupt handler above (have to do this to work for multiple i2c peripheral units on the LPC1768
void vtI2C1Isr(void) {
	// Log the I2C status code
	vtITMu8(vtITMPortI2C1IntHandler,((devStaticPtr[1]->devAddr)->I2STAT & I2C_STAT_CODE_BITMASK));
	vtI2CIsr(devStaticPtr[1]->devAddr,&(devStaticPtr[1]->binSemaphore));
}
// Simply pass on the information to the real interrupt handler above (have to do this to work for multiple i2c peripheral units on the LPC1768
void vtI2C2Isr(void) {
	vtI2CIsr(devStaticPtr[2]->devAddr,&(devStaticPtr[2]->binSemaphore));
}

#include "comm.h"
#include "klcd.h"

// This is the actual task that is run
//   We expect only one to be run the entire time, ie we only support one I2C connection.
static portTASK_FUNCTION( vI2CMonitorTask, pvParameters )
{
	
	// Get the i2c structure for this task/device
	vtI2CStruct *devPtr = (vtI2CStruct *) pvParameters;
	I2C_M_SETUP_Type tx;

	char outBuf[MAX_OUT_SIZE];
	// All the space we need for header + frame size
	char inBuf[15];
	
	for (;;) {
		RoverAction last = nextCommand((int*)&tx.tx_length, outBuf);
		tx.tx_data = (unsigned char*)outBuf;
		
		// TODO: See which ones of these I can hoist above the for loop
		tx.sl_addr7bit = PICMAN_I2C_ADDR;
		tx.rx_data = (unsigned char*)inBuf;
		tx.rx_length = sizeof inBuf;
		tx.retransmissions_max = 3;
		tx.retransmissions_count = 0;	 // this *should* be initialized in the LPC code, but is not for interrupt mode
		int ret = I2C_MasterTransferData(devPtr->devAddr, &tx, I2C_TRANSFER_INTERRUPT);
		// Block until the I2C operation is complete -- we *cannot* overlap operations on the I2C bus...
		FAILIF(xSemaphoreTake(devPtr->binSemaphore,portMAX_DELAY) != pdTRUE);
		
		if (ret) {
		 	// TODO: Check this value too?
		}
		
		gotData(last, inBuf);
	}
}


