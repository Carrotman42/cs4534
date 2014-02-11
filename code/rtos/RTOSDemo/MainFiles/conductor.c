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
#include "conductor.h"

/* The i2cTemp task. */
static portTASK_FUNCTION_PROTO( vConductorUpdateTask, pvParameters );


#include "armcommon.h"
#include "brain_rover.h"
#include "klcd.h"

#define LCD_REFRESH_RATE
#ifdef LCD_REFRESH_RATE					 
TIMER_PROTOTYPE_NOARG(CopyToLCDTimer, 250/portTICK_RATE_MS);
#endif	
/*-----------------------------------------------------------*/
// Public API
void vStartConductorTask(vtConductorStruct *params,unsigned portBASE_TYPE uxPriority, vtI2CStruct *i2c,vtTempStruct *temperature)
{
	/* Start the task */
	portBASE_TYPE retval;
	params->dev = i2c;
	params->tempData = temperature;
	if ((retval = xTaskCreate( vConductorUpdateTask, ( signed char * ) "Conductor", 1000, (void *) params, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
	
#ifdef LCD_REFRESH_RATE
	START_TIMER(MakeCopyToLCDTimer(), 0);
#endif
}

// End of Public API
/*-----------------------------------------------------------*/


#define SAVED_SIZE SIGNAL_SAMPLES
static char saved[SAVED_SIZE] = {0};
static int savedPos = 0;

void copyToLCD() {
	static int times = 0;
	times = (times + 1) % 1;
	if (times == 0) {
		// Copy the full saved buffer to the signal buffer
		SignalLCDMsg* msg = LCDgetSignalBuffer(); 
		char* dest = &(msg->data[0]);
		char* end = &(msg->data[SAVED_SIZE]);
		char* src = &saved[0];
		while (dest != end) {
			*dest++ = *src++;
		}
		LCDcommitBuffer(msg);
	}
}

int handleAD(sensorADData* data, int len) {
	
	/* COPY/WRAP code
	{
		sensorADData* in = data;
		sensorADData* end = in+len;
		while (in != end) {
			saved[savedPos] = (char)((*in++).data);
			savedPos = (savedPos + 1) % SAVED_SIZE;
		}
	}// */
	
	// Attempt at trigger noticing
	static int waiting = 1;
	static char lastNum = 0;
	
	{
		sensorADData* in = data;
		sensorADData* end = data + len;
		if (waiting) {
			do {
				char cur = (char)((*in++).data);
				waiting = (lastNum - cur) > 0;
				lastNum = cur;
				
				if (in == end) {
					// We were looking for a trigger but didn't find one in this whole set of
					//    samples. So let's stop and wait until the next set of samples.
					//    (there's no reason to draw on the LCD if we didn't actually write down
					//    any new samples).
					return 0;
				}
			} while (waiting);
			// When we're done waiting for a trigger we should start drawing at the leftmost side:
			savedPos = 0;
		}
		while (in != end) {
			saved[savedPos] = (char)((*in++).data);
			if (++savedPos >= SAVED_SIZE) {
				// Got to the end of the buffer. We should stop processing this set of samples
				//    and should just spit out the signal buffer
				waiting = 1;
				break;
			}
		}
	} // */
	

#ifndef LCD_REFRESH_RATE
	copyToLCD();
#endif
	return 0;
	#undef SAVED_SIZE
}

#ifdef LCD_REFRESH_RATE
TIMER_FUNC_NOARG(CopyToLCDTimer) {
	copyToLCD();
} ENDTIMER
#endif

RoverMsgRouter Conductor = {
	handleAD,
};


// This is the actual task that is run
static portTASK_FUNCTION( vConductorUpdateTask, pvParameters )
{
	uint8_t rxLen, status;
	uint8_t buffer[700];
	// Get the parameters
	vtConductorStruct *param = (vtConductorStruct *) pvParameters;
	// Get the I2C device pointer
	vtI2CStruct *devPtr = param->dev;
	uint8_t recvMsgType;

	// Like all good tasks, this should never exit
	for(;;)
	{
		// Wait for a message from an I2C operation
		if (vtI2CDeQ(devPtr,vtI2CMLen,buffer,&rxLen,&recvMsgType,&status) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		if (status == ERROR) {
			continue;
		}

		/*{
			static int times = 0;
			times++;
			char b[10];
			char *c = b;
			*c++ = 'C';
			*c++ = '0' + times%10;
			*c++ = '0' + recvMsgType;
			*c++ = '0' + rxLen;
			*c++ = 0;
			LCDwriteLn(0, b);
		}*/
		
		
		static char ebuf[10];
		char* err;
		// Test the error code from unpacking:
		int ret = unpackRoverMsg((char*)buffer, rxLen, &Conductor);
		switch (ret) {
			case 0:
				err = NULL; //no error
				break;
			default:
				ebuf[0] = 'U';
				ebuf[1] = '0' + ret;
				ebuf[2] = 0;
				err = ebuf;
				break;
			case -1:
				err = "Unknown msgid";
				break;
			case -2:
				err = "Unknown sensorid";
				break;
			case -3:
				err = "payLen!=actLen";
				break;
		}
		if (err) {
			LCDwriteLn(1, err);
		} else {
			//LCDwriteLn(1, "noerr");
		}
	}
}













		 