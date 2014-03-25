
#include "armcommon.h"

#include "map.h"
#include "frames.h"

static Map map;
static Memory mem;

void *memset(void*, int, size_t);
void initMap() {
	memset(&map, 0, sizeof(Map));
	memset(&mem, 0, sizeof(Memory));
}

inline void mapGetMemory(Memory* dest){
	*dest = mem;
}
inline void mapGetMap(Map* dest) {
	*dest = map;
}
#include "klcd.h"
void mapReportNewFrame(char* frame) {
	LCDwriteLn(7, "Got new frame");
}