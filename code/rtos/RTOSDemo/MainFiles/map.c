
#include "armcommon.h"

#include "map.h"

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
/*void recordFrames(int len, Frame* frame) {
	
}*/