
#include "armcommon.h"

#include "map.h"
#include "frames.h"
#include "semphr.h"

static Map map;
static Memory mem;
static xSemaphoreHandle dataSem;

// Uses the current memory to make a guess at where the wall in the course is.
//   It takes into account the X,Y position as well as sensors.
void mapMark() {
	
}

#define FSM_TASKS
#include "tasks.h"

//void *memset(void*, int, size_t);
void InitMind() {
	void initFsm();
	initFsm();
	//memset(&map, 0, sizeof(Map));
	//memset(&mem, 0, sizeof(Memory));
	mem.X = MAP_WIDTH/2*MAP_RESOLUTION;
	mem.Y = (MAP_WIDTH-20)*MAP_RESOLUTION;
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

#define TICK_PRESCALE 75

void mapReportNewFrame(int colorSensed, char* frame) {
	//LCDwriteLn(2, "Got new frame");
	
	if (colorSensed) {
		TriggerEvent(COLOR_SENSOR_TRIGGERED);
	}
	
	// Read the data in the correct form
	Frame *f = (Frame*)frame;
	int countDone = 0;
	
	LOCK
		mem.Forward = f->ultrasonic;
		mem.Right1 = f->IR1;
		mem.Right2 = f->IR2;
		
		if (mem.newDir) {
			mem.newDir = 0;
			// Ignore encorder values for the first frames after turning.
		} else {
			int ticks = (u2_8to16(f->encoderRight) + u2_8to16(f->encoderLeft))/TICK_PRESCALE;
			
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
	UNLOCK
	
	if (countDone) {
		TriggerEvent(TICK_COUNTING_DONE);
	}
	
	TriggerEvent(NEW_SENSOR_DATA);
	// Maybe only do this every few loops?
	//mapMark();
	
	// We can read mem safely because this is the only task that
	//   ever modifies mem
	
	bBuf(100);
	bChar('(');
	bWord(mem.X);
	bChar(',');
	bWord(mem.Y);
	bStr("); tC=");
	bWord(mem.tCount);
	bPrint(14);	
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
	int x, y;
	LOCK
		x = mem.X;
		y = mem.Y;
	UNLOCK
	
	
	bBuf(100);
	bStr("Timer started     started at (");
	bWord(x);
	bChar(',');
	bWord(y);
	bChar(')');
	bPrint(9);
}
void mapStopTimer() {
	LCDwriteLn(9, "Timer stopped");
}
void mapRegisterTick(int x) {
	LOCK
		mem.tCount = x;
	UNLOCK
}


