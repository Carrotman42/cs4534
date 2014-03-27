
#include "armcommon.h"

#include "map.h"
#include "frames.h"
#include "semphr.h"

static Map map;
static Memory mem;
static xSemaphoreHandle dataSem;

#define FSM_TASKS
#include "tasks.h"

//void *memset(void*, int, size_t);
void InitMind() {
	void initFsm();
	initFsm();
	//memset(&map, 0, sizeof(Map));
	//memset(&mem, 0, sizeof(Memory));
	vSemaphoreCreateBinary(dataSem);
	FAILIF(dataSem == NULL);
	
	StartPathFindingFSM();
}

inline void mapGetMemory(Memory* dest){
	*dest = mem;
}
inline void mapGetMap(Map* dest) {
	*dest = map;
}
#include "klcd.h"
#include "frames.h"
#include "fsm.h"
void mapReportNewFrame(char* frame) {
	LCDwriteLn(2, "Got new frame");
	// Read the data in the correct form
	Frame *f = (Frame*)frame;
	FAILIF(xSemaphoreTake(dataSem, portMAX_DELAY) != pdTRUE);
	mem.Forward = f->ultrasonic;
	mem.Right1 = f->IR1;
	mem.Right2 = f->IR2;
	FAILIF(xSemaphoreGive(dataSem) != pdTRUE);
	
	TriggerEvent(NEW_SENSOR_DATA);
}


void mapStartTimer() {
	LCDwriteLn(9, "Timer started");
}
void mapStopTimer() {
	LCDwriteLn(9, "Timer stopped");
}


