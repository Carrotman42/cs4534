
// This file contains functions for processing data from different sensors.

#include "common.h"
#include "armcommon.h"
#include "sensors.h"
#include "vtI2C.h"	  
#include "i2cTemp.h"
// TODO: Rewrite this to handle multiple inputs. Eventually it will be the tasks listening to wifi.
#include "conductor.h"

#include "brain_rover.h"

static vtI2CStruct vtI2C0;
static vtTempStruct vtTemp;
static vtConductorStruct conduct;

#define SENSOR_TASKS
#include "tasks.h"
#include "klcd.h"



#define mainI2CMONITOR_TASK_PRIORITY			( tskIDLE_PRIORITY)
void StartSensorTasks() {
	// Temp sensor buffer (for milestone 1)
	FAILIF(vtI2CInit(&vtI2C0,0,mainI2CMONITOR_TASK_PRIORITY,100000) != vtI2CInitSuccess);

	//START_TIMER(MakePumpSensor(), 0);
	StartPumpSensor();
	vStartConductorTask(&conduct, tskIDLE_PRIORITY, &vtI2C0, &vtTemp);
}



void SendADRequest(vtI2CStruct* dest) {
	BrainMsg msg;
	packBrainMsgRequest(&msg, 0);
	
	if (vtI2CEnQ(dest, 1, 0x4F, sizeof(BrainMsg), (unsigned char*)(&msg), 103) != pdTRUE) {
		VT_HANDLE_FATAL_ERROR(0);
	}
}



TASK_FUNC_NOARG(PumpSensor) {
	for (;;) {
		DBGbit(1, 1);
		SendADRequest(&vtI2C0);
		DBGbit(1, 0);
		vTaskDelay(100/portTICK_RATE_MS);
	}
} ENDTASK



