
#ifndef MAP_H_INC
#define MAP_H_INC

// Defines the "armunits" per map "tile" for use of discretization.
//   It's a guess right now, but we need to choose a value that allows us
//   to have a good balance of precision and memory/processing required.
#define MAP_RESOLUTION 10
#define MAP_WIDTH 64

typedef enum {
	Right = 0, // Initial value
	Up = 1,
	Left = 2,
	Down = 3,
} Dir;

//All values in "armunits"
typedef struct {
	// Best guess at location with top-left being (0, 0)
	int X, Y;
	
	// Current direction
	Dir dir;
	
	// Will be true if "dir" was recently changed. This means that the next set of
	//    encoder datas should be ignored since they will be from the turn itself.
	int newDir : 1;
	
	// The last values for each sensor.
	int Forward;
	int Right2;
	int Trend;
	
	// A countdown of ticks left before 
	unsigned int tCount;
} Memory;

void mapReportNewFrame(int colorSensed, char* frame);
void mapReportTurn(int dir);
void InitMind();

typedef struct {
	uint8_t data[MAP_WIDTH*MAP_WIDTH / 4];
} Map;

// Returns the confidence of the rover that there is a wall in this map point between 0 and 3.
//  Valid values for x and y are [0,MAP_WIDTH)
int mapTest(int x, int y);

// Same as mapTest, but reads from the given map
int mapTestMap(Map*m, int x, int y);

// Copy the real map into this destination map for safekeeping
void mapGetMap(Map*dest);
// Return the pointer to the real map. Do not modify this map!
Map* mapMapPtr();
void mapGetMemory(Memory*dest);
int mapLap();
void mapGetLap(char*lap1, char*lap2);
void mapRegisterTick(int x);


#endif


