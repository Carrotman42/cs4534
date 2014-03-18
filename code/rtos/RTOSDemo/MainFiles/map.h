
#ifndef MAP_H_INC
#define MAP_H_INC

// Defines the "armunits" per map "tile" for use of discretization.
//   It's a guess right now, but we need to choose a value that allows us
//   to have a good balance of precision and memory/processing required.
#define MAP_RESOLUTION 10
#define MAP_WIDTH 48

//All values in "armunits"
typedef struct {
	// Best guess at location
	int X, Y;
	
	// The last values for each sensor.
	int Forward;
	int Right1, Right2;
} Memory;

#include "frames.h"
void mapRecordFrames(int len, Frame* frame);

void initMap();

typedef struct {
	uint8_t data[MAP_WIDTH*MAP_WIDTH / 8];
} Map;

void mapGetMap(Map*dest);
void mapGetMemory(Memory*dest);


#endif


