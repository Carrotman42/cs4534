
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
	/*MapInfo(x/MAP_RESOLUTION, y/MAP_RESOLUTION, ind, shift);
	
	uint8_t b = map.data[ind];
	int prev = (b >> shift) & 3;
	if (prev < 2) { // 3 means most confident already, don't modify it
		// We don't need to lock because there is no "inconsistent" map. See other comment about same subject.
		map.data[ind] = (b & (~(3 << shift))) | ((prev + 1) << shift);
	}*/
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
	bPrint(12);*/
}


#define LOCK FAILIF(xSemaphoreTake(dataSem, 1000/portTICK_RATE_MS) != pdTRUE);
#define UNLOCK FAILIF(xSemaphoreGive(dataSem) != pdTRUE);

// Uses the current memory to make a guess at where the wall in the course is.
//   It takes into account the X,Y position as well as sensors.
void mapMark() {
	MapInfo(mem.X/MAP_RESOLUTION, mem.Y/MAP_RESOLUTION, ind, shift);
	map.data[ind] = map.data[ind] | (3 << shift);

	int dir;
	LOCK
		dir = mem.dir;
	UNLOCK
	// Only record the data when it's relatively close
	if (mem.Forward < 100) {
		mapMarkSensor(mem.Forward, dir);
	}
	if (mem.Right2 < 100) {
		mapMarkSensor(mem.Right2, (dir + 3) % 4);
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
	mem.Y = (MAP_WIDTH-10)*MAP_RESOLUTION;
	dataSem = xSemaphoreCreateMutex();
	FAILIF(dataSem == NULL);
	
	StartPathFindingFSM();
}

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
static int epoch, lap1, lap2;
void mapGetLap(char*l1, char*l2) {
	if (!epoch) {
		*l1 = *l2 = 0;
	} else {
		*l1 = ((lap1 ? lap1 : xTaskGetTickCount()) - epoch) / configTICK_RATE_HZ ;
		*l2 = !lap1 ? 0 : ((lap2 ? lap2 : xTaskGetTickCount()) - lap1) / configTICK_RATE_HZ ;
	}
}
int mapLap() {
	if (!epoch) {
		epoch = xTaskGetTickCount();
		dbg(Lap, 1);
		return 1;
	} else if (!lap1) {
		lap1 = xTaskGetTickCount();
		dbg(Lap, 2);
		return 2;
	} else if (!lap2) {
		lap2 = xTaskGetTickCount();
		dbg(Lap, -1);
		return 3;
	} else {
		LCDwriteLn(14, "Too many laps!");
		dbg(Lap, 99);
		return 4;
	}
}
   
#include <math.h>
int valToArm(int v) {
	return (int)(2*41.543 * pow(((float)(v)*3.1/255. + 0.30221), -1.5281));
}

#define OKS 3

static int IRpast[OKS];
void clearIR(int v) {
	int i;
	for (i = 0; i < OKS; i++) {
		IRpast[i] = v;
	}
}

	/* *ir = valToArm(v);
	{
		bBuf(50);
		bStr("RAW IR:");
		bByte(v);
		bStr("; CM IR:");
		bByte(*ir);
		bPrint(1);
	}
	return 1;*/
	
	
	
int getIR(int v, int*ir) {
	int oks = 0;
	int i;
	for (i = 0; i < OKS; i++) {
		int cur = IRpast[i];
		int d = (v - cur) / (OKS - i);
		
		if (d >= -3 && d <= 3) {
			oks++;
		}
		if (i != 0) {
			IRpast[i-1] = cur;
		}
	}
	IRpast[OKS-1] = v;
	
	if (oks <= OKS / 2) {
		v = valToArm(v);
		bBuf(40);
		bStr("Ig:");
		bByte(v);
		bStr(" || ");
		for (i = 0; i < OKS; i++) {
			int pp = valToArm(IRpast[i]);
			bChar(',');
			bByte(pp);
		}
		bPrint(1);
		return 0;
	}
	
	*ir = valToArm(v);
	return 1;
}

#define GoodIrMem 5
#define TrendScale 10
#define TrendThresh (5 * TrendScale)
int getTrend(int ir) {
	static int GoodIrs[GoodIrMem + 1] = {0};
	static int Irs[3] = {0};
	static int irPos = 0;
	
	Irs[irPos] = ir;
	irPos++;
	if (irPos < 3) {
		return 0;
	}
	irPos = 0;
	
	#define swap(a, b) {\
		int t = a; \
		a = b; \
		b = t; \
	}
	int x = Irs[0], y = Irs[1], z = Irs[2];
	if (x < y) {
		if (z < x) {
			swap(x,z);
		}
	} else {
		if (y < z) {
			swap(x, y);
		} else {
			swap(x, z);
		}
	}
	if (z < y) {
		swap(y, z);
	}
	
	int trendTot = 0;
	int trendCount = 0;
	int good = 1;
	int last = -1;
	GoodIrs[0] = y;
	int i;
	for (i = 0; i < GoodIrMem; i++) {
		int d = GoodIrs[i] - GoodIrs[i+1];
		
		if (last != -1) {
			int diff = (d - last) * TrendScale;
			
			if (diff <= -TrendThresh || diff >= TrendThresh) {
				good = 0;
			} else {
				trendTot += d;
				trendCount++;
			}
		}
		last = d;
		
		GoodIrs[i+1] = GoodIrs[i];
	}
	
	if (good) {
		return trendTot / trendCount;
	} else {
		return 0;
	}
}

void mapReportNewFrame(int colorSensed, int turning, char* frame) {
	//LCDwriteLn(2, "Got new frame");
	
	if (colorSensed) {
		TriggerEvent(COLOR_SENSOR_TRIGGERED);
	}
	
	// Read the data in the correct form
	Frame *f = (Frame*)frame;
	int countDone = 0;
	
	// Do IR calculations before getting the lock
	
	// IGNORE IR1, FOR NOW
	//int ir1 = valToCm(f->IR1);
	
	// Hacky, woo!
	static int wasTurning = 0;
	
	int ir2, trend = 0;
	if (!turning) {
		if (wasTurning) {
			wasTurning = 0;
			clearIR(f->IR2);
		}
		static int lastIR = 0;
		
		// Pre-calculate the linear value for the IR so that we don't hold the lock during a "long" math op
		if (!getIR(f->IR2, &ir2)) {
			ir2 = mem.Right2;
		}
		
		int d = lastIR - ir2;
		if (d <= -10 || d >= 10) {
			// We found a wall! Clear that trend:
			// nop
		}
		lastIR = ir2;
	} else {
		wasTurning = 1;
		ir2 = mem.Right2;
	}
	
	trend = getTrend(ir2);
	
	LOCK
		mem.Forward = f->ultrasonic;
		//mem.Right1 = 0;
		mem.Right2 = ir2;
		mem.Trend = trend;
		
		int ticks = (u2_8to16(f->encoderRight) + u2_8to16(f->encoderLeft))/2/(80/MAP_RESOLUTION);
		
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
	UNLOCK
	
	if (countDone) {
		TriggerEvent(TICK_COUNTING_DONE);
	}
	
	TriggerEvent(NEW_SENSOR_DATA);
	mapMark();
	
	// We can read mem safely because this is the only task that
	//   ever modifies mem
	
	/*if (mem.tCount) {
		bBuf(40);
		bStr("TICK COUNTDOWN=");
		bWord(mem.tCount);
		bPrint(15);
	} else if (countDone) {	
		bBuf(40);
		bStr("                        ");
		bPrint(15);
	}*/

}

// Called to record that the rover has finished the turn with the given direction. dir should be -1 for right and 1 for left.
void mapReportTurn(int dir) {
	if (dir != 0) {
		Dir d;
		DBGbit(0, 1);
		LOCK
			d = (mem.dir = ((mem.dir + 4 + dir) % 4));
		UNLOCK
		DBGbit(0, 0);
	}

	TriggerEvent(TURN_COMPLETE);
	
	/*bBuf(10);
	bStr("Turned ");
	bChar((dir == 1 ? 'L' : 'R'));
	bStr("; now ");
	bChar(drawDir(d));
	bPrint(12);*/
}

void mapRegisterTick(int x) {
	LOCK
		mem.tCount = x;
	UNLOCK
}

