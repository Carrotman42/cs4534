
#include "armcommon.h"

#include "map.h"
#include "frames.h"
#include "semphr.h"


#include "klcd.h"

static Map map;
static Memory mem;
static xSemaphoreHandle dataSem;

// 4 map entries are packed into each byte

// This macro calculates the index and shift for map access and puts them into newly created int variables
//    as designated by the names of 'ind' and 'shift'
#define MapInfo(x,y, ind, shift) \
	int ind = x*MAP_WIDTH+y;   \
	int shift = (ind & 3) << 1; \
	ind >>= 2;

inline int mapTestMap(Map*m, int x, int y) {
	MapInfo(x, y, ind, shift);
	return (m->data[ind] >> shift) & 3;
}
inline int mapTest(int x, int y) {
	return mapTestMap(&map, x, y);
}

// Increments the confidence at the given coordinate position. x and y
//    are both in armunits (ie not already divided MAP_RESOLUTION)
void mapInc(int x, int y) {
	// This is called in the only task which modifies mem/map, so we don't need to lock it to read them
	MapInfo(x/MAP_RESOLUTION, y/MAP_RESOLUTION, ind, shift);
	
	uint8_t b = map.data[ind];
	int prev = (b >> shift) & 3;
	if (prev < 2) { // 3 means most confident already, don't modify it
		// We don't need to lock because there is no "inconsistent" map. See other comment about same subject.
		map.data[ind] = (b & (~3 << shift)) | ((prev + 1) << shift);
	}
}

void mapMarkSensor(int val, Dir dir) {
	int x = mem.X, y = mem.Y;
	switch (dir) {
		case Right: x += val; break;
		case Up: y -= val; break;
		case Left: x -= val; break;
		case Down: y += val; break;
		default: FATAL(dir);
	}
	mapInc(x, y);
	/*bBuf(40);
	bChar('(');
	bWord(x);
	bChar(',');
	bWord(y);
	bChar(')');
	bPrint(13);	*/
}

// Uses the current memory to make a guess at where the wall in the course is.
//   It takes into account the X,Y position as well as sensors.
void mapMark() {
	MapInfo(mem.X/MAP_RESOLUTION, mem.Y/MAP_RESOLUTION, ind, shift);
	map.data[ind] = map.data[ind] | (3 << shift);
	
	// Only record the data when it's relatively close
	if (mem.Forward < 100) {
		mapMarkSensor(mem.Forward, mem.dir);
	}
	if (mem.Right1 < 100) {
		mapMarkSensor(mem.Right1, (mem.dir + 3) % 4);
	}
	
}

#define FSM_TASKS
#include "tasks.h"

//void *memset(void*, int, size_t);
// Initializes all aspects of the "mind" of the rover, dealing with pathfinding and knowing where itself is.
void InitMind() {
	void initFsm();
	initFsm();
	//memset(&map, 0, sizeof(Map));
	//memset(&mem, 0, sizeof(Memory));
	mem.X = MAP_WIDTH/2*MAP_RESOLUTION;
	mem.Y = (MAP_WIDTH-25)*MAP_RESOLUTION;
	dataSem = xSemaphoreCreateMutex();
	FAILIF(dataSem == NULL);
	
	StartPathFindingFSM();
}

#define LOCK FAILIF(xSemaphoreTake(dataSem, portMAX_DELAY) != pdTRUE);
#define UNLOCK FAILIF(xSemaphoreGive(dataSem) != pdTRUE);

// Returns a copy of where the rover thinks it is into dest.
inline void mapGetMemory(Memory* dest){
	// We need a lock because otherwise mem could be read here in the middle of it being written to elsewhere and
	//    we could get a partial representation of reality.
	LOCK
		*dest = mem;
	UNLOCK
}
// Returns a copy of the current map into dest
inline void mapGetMap(Map* dest) {
	//  We don't need a lock because there's no such thing as an "inconsistent" map (even when the map is being changed there is no point
	//   during an update during which the map could be in a non-readable state)
	*dest = map;
}

Map* mapMapPtr() {
	return &map;
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
		
		mem.newDir = 0;
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
	UNLOCK
	
	if (countDone) {
		TriggerEvent(TICK_COUNTING_DONE);
	}
	
	TriggerEvent(NEW_SENSOR_DATA);
	mapMark();
	
	// We can read mem safely because this is the only task that
	//   ever modifies mem
	
	bBuf(40);
	if (mem.tCount) {
		bStr("TICK COUNTDOWN=");
		bWord(mem.tCount);
	} else {
		bStr("                   ");
	}
	bPrint(15);
	// 'b' is reset correctly by above macro.
	bChar('(');
	bWord(mem.X);
	bChar(',');
	bWord(mem.Y);
	bChar(')');
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

