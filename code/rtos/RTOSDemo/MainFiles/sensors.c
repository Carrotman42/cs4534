
// This file contains functions for processing data from different sensors.

#include "common.h"
#include "sensors.h"
#include "vtI2C.h"	  
#include "i2cTemp.h"
// TODO: Rewrite this to handle multiple inputs. Eventually it will be the tasks listening to wifi.
#include "conductor.h"



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

	vStarti2cTempTask(&vtTemp, tskIDLE_PRIORITY, &vtI2C0);
	START_TIMER(MakePumpSensor(), 0);
	vStartConductorTask(&conduct, tskIDLE_PRIORITY, &vtI2C0, &vtTemp);
}

TIMER_FUNC_NOARG(PumpSensor) {
	static int count = 0;
	SendTempTimerMsg(&vtTemp,PumpSensorPERIOD,0);
	if ((count++ % 40) == 0) {
		char buf[2] = {(count/10)%10 + '0', 0};
		LCDwriteLn(3, buf);
	}
} ENDTIMER







