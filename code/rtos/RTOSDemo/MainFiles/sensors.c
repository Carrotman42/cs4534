
#include "common.h"
#include "sensors.h"
#include "vtI2C.h"	  
#include "i2cTemp.h"
// TODO: Rewrite this to handle multiple inputs. Eventually it will be the tasks listening to wifi.
#include "conductor.h"

// This file will change as we progress, but the idea is to minimize API changes. It will eventually look nice, but is
//    hackish now.

static vtI2CStruct vtI2C0;
static vtTempStruct vtTemp;
static vtConductorStruct conduct;

#define SENSOR_TASKS
#include "tasks.h"
#include "klcd.h"




TASK_PROTOTYPE_NOARG(PumpSensor, 1000, tskIDLE_PRIORITY);

#define mainI2CMONITOR_TASK_PRIORITY			( tskIDLE_PRIORITY)
void StartSensorTasks() {
	// Temp sensor buffer (for milestone 1)
	FAILIF(vtI2CInit(&vtI2C0,0,mainI2CMONITOR_TASK_PRIORITY,100000) != vtI2CInitSuccess);

	vStarti2cTempTask(&vtTemp, tskIDLE_PRIORITY, &vtI2C0);
	StartPumpSensor();
	vStartConductorTask(&conduct, tskIDLE_PRIORITY, &vtI2C0, &vtTemp);
}
				  
#define tempWRITE_RATE_BASE	( ( portTickType ) 500 / portTICK_RATE_MS)
TASK_FUNC_NOARG(PumpSensor) {
	int count = 0;	
	while(1) {
		vTaskDelay(100/portTICK_RATE_MS);
		SendTempTimerMsg(&vtTemp,tempWRITE_RATE_BASE,0);
		if (count++ % 40 == 0) {
			char buf[2] = {(count/10)%10 + '0', 0};
			LCDwriteLn(3, buf);
			//FATAL(0);
		}
	}
} ENDTASK








