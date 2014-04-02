
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
	dataSem = xSemaphoreCreateMutex();
	FAILIF(dataSem == NULL);
	
	StartPathFindingFSM();
}

#define LOCK FAILIF(xSemaphoreTake(dataSem, portMAX_DELAY) != pdTRUE);
#define UNLOCK FAILIF(xSemaphoreGive(dataSem) != pdTRUE);

inline void mapGetMemory(Memory* dest){
	LOCK
		*dest = mem;
	UNLOCK
}
inline void mapGetMap(Map* dest) {
	*dest = map;
}
#include "klcd.h"
#include "frames.h"
#include "fsm.h"

char drawDir(Dir dir) {
	switch (dir) {
		case Right: return '>';
		case Up: return '^';
		case Left: return '<';
		case Down: return 'v';
		default: return 'X';
	}
}

void mapReportNewFrame(char* frame) {
	//LCDwriteLn(2, "Got new frame");
	// Read the data in the correct form
	Frame *f = (Frame*)frame;
	
	int X, Y, tCount, dir, countDone = 0;
	
	LOCK
		mem.Forward = f->ultrasonic;
		mem.Right1 = f->IR1;
		mem.Right2 = f->IR2;
		
		if (mem.newDir) {
			mem.newDir = 0;
			// Ignore encorder values for the first frames after turning.
		} else {
			int ticks = (u2_8to16(f->encoderRight) + u2_8to16(f->encoderLeft))/2;
			
			if (mem.tCount > 0) {
				if (mem.tCount > ticks) {
					mem.tCount -= ticks;
				} else {
					mem.tCount = 0;
					countDone = 1;
				}
			}
			
			switch (mem.dir) {
				case Right:
					mem.X += ticks;
					break;
				case Up:
					mem.Y -= ticks;
					break;
				case Left:
					mem.X -= ticks;
					break;
				case Down:
					mem.Y += ticks;
					break;
			}
		}
		X = mem.X;
		Y = mem.Y;
		tCount = mem.tCount;
		dir = mem.dir;
	UNLOCK
	
	if (countDone) {
		TriggerEvent(TICK_COUNTING_DONE);
	}
	
	TriggerEvent(NEW_SENSOR_DATA);
	
   {
	bBuf(100);
	bChar('(');
	bWord(X);
	bChar(',');
	bWord(Y);
	bStr("); tC=");
	bWord(tCount);
	bPrint(14);
   }
    bBuf(100);
	bChar(drawDir(dir));
	bPrint(13);		
}

// Called to record that the rover has finished the turn with the given direction. dir should be -1 for right and 1 for left.
void mapReportTurn(int dir) {
	Dir d;
	LOCK
		d = (mem.dir = (mem.dir + 4 + dir) % 4);
		mem.newDir = 1;
	UNLOCK

	TriggerEvent(TURN_COMPLETE);
	bBuf(10);
	bStr("Turned ");
	bChar((dir == 1 ? 'L' : 'R'));
	bStr("; now ");
	bChar(drawDir(d));
	bPrint(12);
}

void mapStartTimer() {
	LCDwriteLn(9, "Timer started");
}
void mapStopTimer() {
	LCDwriteLn(9, "Timer stopped");
}
void mapRegisterTick(int x) {
	LOCK
		mem.tCount = x;
	UNLOCK
}


