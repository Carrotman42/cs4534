#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"
#include "semphr.h"
#include "timers.h"
				
#include "armcommon.h"
/* include files. */
#include "vtUtilities.h"
#include "vtI2C.h"
#include "I2CTaskMsgTypes.h"

#include "conductor.h"

#include "brain_rover.h"
#include "klcd.h"

#define COMM_TASKS
#include "tasks.h"


#define LCD_REFRESH_RATE
#ifdef LCD_REFRESH_RATE					 
TIMER_PROTOTYPE_NOARG(CopyToLCDTimer, 250/portTICK_RATE_MS);
#endif	

// Of type BrainMsg
static xQueueHandle toRover;

// Public API
void StartProcessingTasks(vtI2CStruct *i2c) {
	MAKE_Q(toRover, BrainMsg, 2);
	
	StartFromI2C(i2c);
	//StartToI2C(i2c);
	START_TIMER(MakeCopyToLCDTimer(), 0);
}

// End of Public API


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

int ms2Msg(sensorADData* data, int len) {
	aBuf(b, 100);
	aStr(b, "Len: ");
	aByte(b, len);
	aStr(b, "; Data: ");
	aByte(b, data[0].data);
	aChar(b, ' ');
	aByte(b, data[1].data);
	aChar(b, 0);
	aPrint(b, 5);

	// Then just pass the data to try and plot it
	//return handleAD(data, len);
	return 0;
}

RoverMsgRouter Conductor = {
	//For MS1: handleAD,
	ms2Msg,
};

TASK_FUNC(FromI2C, vtI2CStruct, from) {
	uint8_t rxLen;
	uint8_t buffer[255];

	uint8_t msgCount = 0;
	// Like all good tasks, this should never exit
	for(;;) {
		BrainMsg msg;
		packBrainMsgRequest(&msg, 0);

		//vTaskDelay(1000/portTICK_RATE_MS);
		int q = ki2cReadReq(from, 0x10, msg, buffer, sizeof buffer, &rxLen);
		msgCount++;
		aBuf(a, 100);
		aStr(a, "I2C ret: ");
		aByte(a, q);
		aStr(a, "; msgCount: ");
		aByte(a, msgCount);
		aChar(a, 0);
		aPrint(a, 7);

		// Attempt to read
		if (ERROR == q) {
			LCDwriteLn(10, "I2C read err");
			continue;
		}
		
		// Test the error code from unpacking:
		int ret = unpackRoverMsg((char*)buffer, rxLen, &Conductor);
		
		static char ebuf[3];
		char* err;
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
			LCDwriteLn(1, "noerr");
		}
	}												 
} ENDTASK










		 