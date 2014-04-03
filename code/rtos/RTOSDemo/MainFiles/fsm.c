
#include "fsm.h"

//recv will get events 
//these events can be 
//1) new sensor data (us, ir1, ir2)
//2) turn complete,
//3) tick counting done (from registerTickListener(int x)), 
//4) color sensor triggered

#define WAIT_START_LINE 1 //also continues to move forward
#define WAIT_EVENT 2
#define TURN_STALL 3
#define WAIT_TICKS 4
#define END 7 //picman handles stop frame data and stop moving


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

static int currentstate = WAIT_START_LINE;
static int startCrossed = 0; //remember if the start has been initially crossed

void debug(int line, char* info);

PATH_FINDING_DECL {
	#define GO_SLOW moveForward(50)
	GO_SLOW; //always start moving forward initially.
	currentstate = WAIT_EVENT;
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
			LCDrefreshMap();
		}
		
		#define Remember(name) Memory name; mapGetMemory(&name)
		#define CHECK_FRONT(mem) \
			if(mem.Forward <= 50){\
				stop(); \
				turnCCW(90); \
				currentstate = TURN_STALL; \
			}
		
		//debug(event, "Got event");
		switch(currentstate){
			case WAIT_EVENT:
				switch(event){
					case NEW_SENSOR_DATA: {
						Remember(mem);
						
						CHECK_FRONT(mem)
						else if((mem.Right1 > 30) && (mem.Right2 > 30)){
							registerTickListener(400);
							currentstate = WAIT_TICKS;
						}
						
						break;
					}
					case TURN_COMPLETE:
						break;
					case TICK_COUNTING_DONE:
						break;
					case COLOR_SENSOR_TRIGGERED:
						if(!startCrossed){
							mapStartTimer();
							startCrossed = 1;
							//stay in wait event state
						}
						else{
							mapStopTimer();
							currentstate = END;
							stop();
						}
						break;
					default:
						debug(1, "event error");
						break;
				}
				break;
			case TURN_STALL:
				switch(event){
					case NEW_SENSOR_DATA:
						break;
					case TURN_COMPLETE:
						GO_SLOW;
						currentstate = WAIT_EVENT;
						break;
					case TICK_COUNTING_DONE:
						break;
					case COLOR_SENSOR_TRIGGERED:
						break;
					default:
						debug(1, "event error");
						break;
				}
				break;
			case WAIT_TICKS:
				switch(event){
					case NEW_SENSOR_DATA: {
						Remember(mem);
						
						CHECK_FRONT(mem);
						break;
					}
					case TURN_COMPLETE:
						break;
					case TICK_COUNTING_DONE:{
						Remember(mem);
						
						// Turn right if we don't know where the wall to our right is
						if((mem.Right1 > 30) && (mem.Right2 > 30)){
							turnCW(90);
							currentstate = TURN_STALL;
						} else {
							GO_SLOW;
							currentstate = WAIT_EVENT;
						}
						break;
					}
					case COLOR_SENSOR_TRIGGERED:
						break;
					default:
						debug(1, "event error");
						break;
				}
				break;
			case END:
				debug(1, "in end");
				switch(event){
					case NEW_SENSOR_DATA:
						break;
					case TURN_COMPLETE:
						break;
					case TICK_COUNTING_DONE:
						break;
					case COLOR_SENSOR_TRIGGERED:
						break;
					default:
						debug(1, "event error");
						break;
				}
				break;
			default:
				debug(1, "current state error");
				break;
				
		}
		
	}
} FSM_FOOT


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

void registerTickListener(int x){
	debug(x, "waiting for ticks");
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