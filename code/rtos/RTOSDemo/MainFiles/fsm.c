
#include "fsm.h"


// States and events in fsm.h


#ifdef FSM_TEST
#include <unistd.h>

static Memory mem;

void debug(int line, char* str);
FsmEvent nextEvent();
void moveForward();
void turnCCW();
void turnCW();
void stop();
void mapStartTimer();
void mapStopTimer();
void mapGetMemory(Memory* mem);
void registerTickListener(int x);

#define PATH_FINDING_DECL void pathFindingFSM()
#define FSM_FOOT
#else

#include "armcommon.h"
#include "map.h"
#include "comm.h"

#define PATH_FINDING_DECL TASK_FUNC_NOARG(PathFindingFSM)
#define FSM_FOOT ENDTASK

#define FSM_TASKS
#include "tasks.h"

static xQueueHandle events;
void TriggerEvent(FsmEvent event) {
	SEND(events, event);
}
inline FsmEvent nextEvent() {
	FsmEvent ret;
	//SLEEP(1000);
	//return NEW_SENSOR_DATA;
	
	RECV(events, ret);
	return ret;//*/
}
void initFsm() {
	MAKE_Q(events, FsmEvent, 4);
}
#include "klcd.h"
void debug(int line, char*msg) {
	LCDwriteLn(line, msg);
}
#endif

void debug(int line, char* info);


#if ETHER_EMU==1

#define TOO_FAR 60
#define TOO_CLOSE 20
#define TOO_CLOSE_FRONT 50
#define GO_SLOW moveForward(50)

#else

#define TREND_AWAY_AMT 5
#define WAY_TOO_FAR 106
#define ADJU_THRESH 70
#define TOO_CLOSE 38
#define TOO_CLOSE_FRONT 18

// TODO: Move the speed suggestion to the map rather than the fsm
#define FORWARD() moveForward(1)
#endif

PATH_FINDING_DECL {
	int currentstate = INIT;
	int lastWasData = 0;
	for (;;) {
		int prevstate = currentstate;
		FsmEvent event = nextEvent();
		if (event == NEW_SENSOR_DATA) {
			if (lastWasData) goto skipDbg;
			lastWasData = 1;
		} else {
			lastWasData = 0;
		}
		dbg(NewEvent, event);
skipDbg:
		{
			bBuf(30);
			bStr("State: ");
			bByte(currentstate);
			bStr(", Event: ");
			bByte(event);
			bPrint(5);
		}
		
		switch (event) {
			case TICK_COUNTING_DONE:
				if (currentstate == MOVE_AWAY) {
					turnCW(90);
					currentstate = TURNING_R;
				} else if (currentstate == FIND_WALL) {
					// Give up on finding the wall! Probably will cause it to turn toward
					//     the wall on the next data packet.
					currentstate = WAIT_EVENT;
				}
				break;
			case START:
				FORWARD();
				currentstate = WAIT_EVENT;
				break;
			case RESET_ROVER:
				stop();
				currentstate = INIT;
				break;
			case NEW_SENSOR_DATA: {  
				Memory mem;
				mapGetMemory(&mem);
				int turnok = 1;
				if (currentstate == FIND_WALL) {
					if (mem.Right2 < ADJU_THRESH) {
						// Found the wall
						currentstate = WAIT_EVENT;
					} else {
						turnok = 0;
					}
				} else if (currentstate != WAIT_EVENT && currentstate != WAIT_WALL) {
					break;
				}
			
				int r = mem.Right2;
				if (mem.Forward < TOO_CLOSE_FRONT) {
					// Turn left
					turnCCW(90);
					currentstate = TURNING_L;
				} else if (turnok) {
					// Only do these if we can turn
					if (r >= WAY_TOO_FAR) {
						// Nothing to the right:
						if (currentstate == WAIT_EVENT) {
							// but wait a sec first
							currentstate = WAIT_WALL;
						} else if (currentstate == WAIT_WALL) {
							// Gotta turn right! Probably a chicane
							turnCW(90);
							currentstate = TURNING_R;
						}
					} else if (mem.Trend > TREND_AWAY_AMT) {
						adjuCW(10);
						currentstate = TURNING_ADJ;
					} else if (mem.Trend < -TREND_AWAY_AMT) {
						adjuCCW(10);
						currentstate = TURNING_ADJ;
					} else if (r < TOO_CLOSE) {
						// Too close, initiate LEFT-FORWARD-RIGHT manuever
						turnCCW(90);
						currentstate = MOVE_AWAY;
					}
				}
				break;
			}
			case TURN_COMPLETE:
				switch (currentstate) {
					case TURNING_L:
						currentstate = FIND_WALL;
						mapRegisterTick(50);
						goto finishTurn;
					case TURNING_R:
						currentstate = FIND_WALL;
						mapRegisterTick(50);
						goto finishTurn;
					case TURNING_ADJ:
						// Just an adjust
						currentstate = WAIT_EVENT;
						goto finishTurn;
					case MOVE_AWAY:
						// After we go forward a bit, we'll right again
						mapRegisterTick(10);
						goto finishTurn;
					default:
						dbg(InvalidEvent, TURN_COMPLETE);
				}
				break;

finishTurn:		
				FORWARD();
				break;
			case COLOR_SENSOR_TRIGGERED:
				if (mapLap() == 3) {
					currentstate = END;
					stop();
				}
				break;
		}
		
		
		if (currentstate != prevstate) {
			dbg(StateChange, currentstate);
		}
	}
	
} FSM_FOOT
	
/*
	currentstate = INIT;
	int cur = 0;
	for(;;){
		FsmEvent event = nextEvent();
		{
			bBuf(30);
			bStr("State: ");
			bByte(currentstate);
			bStr(", Event: ");
			bByte(event);
			bPrint(5);
		}
		if (++cur % 5 == 0) {
			//LCDrefreshMap();
		}
		if (event == RESET_ROVER) {
			currentstate = INIT;
			continue;
		}
		
		#define Remember(name) Memory name; mapGetMemory(&name)
		#define CHECK_FRONT(mem) \
			if (mem.Forward <= TOO_CLOSE_SLOW) { \
				GO_SLOW; \
				currentstate = WAIT_EVENT_SLOW; \
			} else if(mem.Forward <= TOO_CLOSE_FRONT){\
				turnCCW(90); \
				currentstate = L_TURN_STALL; \
				registerTickListener(0); \
			}
		
		int prevState = currentstate;
		//debug(event, "Got event");
		switch(currentstate){
			case INIT:
				if (event == START) {
					currentstate = WAIT_EVENT;
					GO_FASTER;
				}
				break;
			case WAIT_EVENT_SLOW:
				switch(event){
					case NEW_SENSOR_DATA: {
						Remember(mem);
						
						if(mem.Forward <= TOO_CLOSE_FRONT){
							turnCCW(90); 
							currentstate = L_TURN_STALL; 
							registerTickListener(0); 
						} else if((mem.Right1 > TOO_FAR) && (mem.Right2 > TOO_FAR)){
						
							currentstate = WAIT_TICKS;
						} else if ((mem.Right1 < TOO_CLOSE) && (mem.Right2 < TOO_CLOSE)) {
							currentstate = ADJ_TURN_STALL;
							// Slightly left
							turnCCW(10);
						}
						
						break;
					}
					case COLOR_SENSOR_TRIGGERED: {
						// If at the beginnning of the 3rd lap, finish and stop
						if (mapLap() == 3) {
							currentstate = END;
							stop();
						}
						break;
					}
					default:
						goto invalid;
				}
				break;
			case WAIT_EVENT:
				switch(event){
					case NEW_SENSOR_DATA: {
						Remember(mem);
						
						CHECK_FRONT(mem)
						else if((mem.Right1 > TOO_FAR) && (mem.Right2 > TOO_FAR)){
							registerTickListener(30);
							currentstate = WAIT_TICKS;
						} else if ((mem.Right1 < TOO_CLOSE) && (mem.Right2 < TOO_CLOSE)) {
							currentstate = ADJ_TURN_STALL;
							// Slightly left
							turnCCW(10);
						}
						
						break;
					}
					case COLOR_SENSOR_TRIGGERED: {
						// If at the beginnning of the 3rd lap, finish and stop
						if (mapLap() == 3) {
							currentstate = END;
							stop();
						}
						break;
					}
					default:
						goto invalid;
				}
				break;
			case L_TURN_STALL:
				switch(event){
					case NEW_SENSOR_DATA:
						// Ignore sensor data here
						break;
					case TURN_COMPLETE:
						GO_FASTER;
						currentstate = WAIT_EVENT;
						break;
					default:
						goto invalid;
				}
				break;
			case ADJ_TURN_STALL:
				switch(event){
					case NEW_SENSOR_DATA:
						// Ignore sensor data here
						break;
					case TURN_COMPLETE:
						GO_FASTER;
						// Just turned slightly, go back to normal
						currentstate = WAIT_EVENT;
						// Maybe? registerTickListener(30);
						break;
					default:
						goto invalid;
				}
				break;
			case R_TURN_STALL:
				switch(event){
					case NEW_SENSOR_DATA:
						// Ignore sensor data here
						break;
					case TURN_COMPLETE:
						GO_FASTER;
						// Just turned right - try and find the wall again
						registerTickListener(50);
						currentstate = WAIT_TICKS;
						break;
					default:
						goto invalid;
				}
				break;
			case WAIT_TICKS:
				switch(event){
					case NEW_SENSOR_DATA: {
						Remember(mem);
						
						CHECK_FRONT(mem);
						break;
					}
					case TICK_COUNTING_DONE:{
						Remember(mem);
						
						// Turn right if we don't know where the wall to our right is
						if((mem.Right1 > TOO_FAR) && (mem.Right2 > TOO_FAR)){
							turnCW(90);
							currentstate = R_TURN_STALL;
						} else {
							GO_FASTER;
							currentstate = WAIT_EVENT;
						}
						break;
					}
					default:
						goto invalid;
				}
				break;
			case END:
				debug(1, "in end");
				switch(event){
					case NEW_SENSOR_DATA:
					case TURN_COMPLETE:
					case TICK_COUNTING_DONE:
					case COLOR_SENSOR_TRIGGERED:
						break;
					default:
						goto invalid;
				}
				break;
			default:
				goto invalid;
				
		}
		
		if (currentstate != prevState) {
			dbg(StateChange, currentstate);
			dbg(BecauseEvent, event);
		}
		
		continue;
		
invalid:
		{
			bBuf(40);
			bStr("Unknown event ");
			bByte(event);
			bStr(" in state ");
			bByte(currentstate);
			bPrint(6);
		}
	}
} FSM_FOOT*/


#ifdef FSM_TEST



#include <stdio.h>
void debug(int line, char* str){
	printf("%d, %s\n", line, str);
}


static int trackStage = -1;

FsmEvent FsmEvent(){
	// Give a delay for the fsm_test
	sleep(1);
	int event = 0;
	trackStage++;
	switch(trackStage){
		case 0: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 1: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 2: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 3: //start crossed
			event = COLOR_SENSOR_TRIGGERED;
			break;
		case 4: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 5: //turn ccw
			event = NEW_SENSOR_DATA;
			break;
		case 6: // turn complete
			event = TURN_COMPLETE;
			break;
		case 7: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 8: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 9: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 10: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 11: //IR1 triggered
			event = NEW_SENSOR_DATA;
			break;
		case 12: //IR2 triggered - wait 400 ticks
			event = NEW_SENSOR_DATA;
			break;
		case 13: //400 ticks done - start turn
			event = TICK_COUNTING_DONE;
			break;
		case 14: //turn done
			event = TURN_COMPLETE;
			break;
		case 15: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 16: //move forward
			event = NEW_SENSOR_DATA;
			break;
		case 17: //start crossed
			event = COLOR_SENSOR_TRIGGERED;
			break;
		default:
			event = 0;
			break;
	}
	return event;
}

void moveForward(){
	debug(5, "moving forward");
}

void turnCCW(){
	stop();
	debug(5, "turning ccw (left)");
}

void turnCW(){
	stop();
	debug(5, "turning cw (right)");
}

void stop(){
	debug(5, "stopping rover");
}

void startTimer(){
	debug(5, "starting timer");
}

void stopTimer(){
	debug(5, "stopping timer");
}


void mapGetMemory(Memory* mem){
	switch(trackStage){
		case 0:
			mem->Forward = 60;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 1:
			mem->Forward = 50;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 2:
			mem->Forward = 40;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 3: //start crossed - should not be called
			debug(1, "asked for memory incorrectly");
			mem->Forward = 30;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 4:
			mem->Forward = 20;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 5: //turn ccw
			mem->Forward = 10;
			mem->Right1 = 5;
			mem->Right2 = 5;
			break;
		case 6: //turn complete
			debug(1, "asked for memory incorrectly");
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 7: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 8: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 9: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 10: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 11: //IR1 triggered
			mem->Forward = 80;
			mem->Right1 = 80;
			mem->Right2 = 10;
			break;
		case 12: //IR2 triggered - wait 400 ticks
			mem->Forward = 80;
			mem->Right1 = 80;
			mem->Right2 = 80;
			break;
		case 13: //400 ticks done - start turn
			debug(1, "asked for memory incorrectly");
			mem->Forward = 80;
			mem->Right1 = 80;
			mem->Right2 = 80;
			break;
		case 14: //turn done
			debug(1, "asked for memory incorrectly");
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 15: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 16: //move forward
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		case 17: //start crossed
			debug(1, "asked for memory incorrectly");
			mem->Forward = 80;
			mem->Right1 = 10;
			mem->Right2 = 10;
			break;
		default:
			mem->Forward = -1;
			mem->Right1 = -1;
			mem->Right2 = -1;
			break;
	}
}

int main(){
	printf("starting...\n");
	pathFindingFSM();
	return 0;
}




#endif