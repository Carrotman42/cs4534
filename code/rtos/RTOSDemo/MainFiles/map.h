
#ifndef MAP_H_INC
#define MAP_H_INC

// Defines the "ticks" per map "tile" for use of discretization.
//   It's a guess right now, but we need to choose a value that allows us
//   to have a good balance of precision and memory/processing required.
#define MAP_RESOLUTION 10
#define MAP_WIDTH 50

//All values in "ticks"
typedef struct {
	// Best guess at location
	int X, Y;
	
	// The last values for each sensor.
	int Forward;
	int Right1, Right2;
} SensorSnapshot;

// NOTE: Every one of these methods wants its value in mapticks, which are going to be
//   different for every sensor. This is not the place for calibration; that place is
//   in the sensor processing file. Keeping it separate helps us make the map
//   a more higher level construct free of physical concerns.

// Sets the value of the front sensor in ticks
void mapSetForward(int val);
// Sets the value of the front right sensor in ticks
void mapSetRight1(int val);
// Sets the value of the back right sensor in ticks
void mapSetRight2(int val);

SensorSnapshot mapGetSnapshot();

#endif