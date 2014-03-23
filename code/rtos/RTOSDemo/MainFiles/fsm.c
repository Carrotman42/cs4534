#include "fsm.h"

//recv will get events 
//these events can be 
//1) new sensor data (us, ir1, ir2)
//2) turn complete,
//3) tick counting done (from registerTickListener(int x)), 
//4) color sensor triggered

#define NEW_SENSOR_DATA 1
#define TURN_COMPLETE 2
#define TICK_COUNTING_DONE 3
#define COLOR_SENSOR_TRIGGERED 4

#define WAIT_START_LINE 1 //also continues to move forward
#define WAIT_EVENT 2
#define TURN_STALL 3
#define START_TICKS 4
#define END 7 //picman handles stop frame data and stop moving


#ifdef FSM_TEST
#include <unistd.h>
#endif



static int currentstate = WAIT_START_LINE;
static int startCrossed = 0; //remember if the start has been initially crossed

static Memory mem;

void debug(int line, char* info);

void pathFindingFSM(){
	moveForward(); //always start moving forward initially.
	currentstate = WAIT_EVENT;
	for(;;){
		#ifdef FSM_TEST
		sleep(1);
		#endif
		int event = RECV();
		debug(10, "Got event");
		switch(currentstate){
			case WAIT_EVENT:
				switch(event){
					case NEW_SENSOR_DATA:
						getMemory(&mem);
						if(mem.Forward <= 10){
							turnCCW();
							currentstate = TURN_CCW;
						}
						else if((mem.Right1 <= 5) && (mem.Right2 <= 5)){
							turnCW();
							currentstate = START_TICKS;
						}
						break;
					case TURN_COMPLETE:
						break;
					case TICK_COUNTING_DONE:
						break;
					case COLOR_SENSOR_TRIGGERED:
						if(!startCrossed){
							startTimer();
							startCrossed = 1;
							//stay in wait event state
						}
						else{
							stopTimer();
							currentstate = END;
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
			case START_TICKS:
				switch(event){
					case NEW_SENSOR_DATA:
						break;
					case TURN_COMPLETE:
						break;
					case TICK_COUNTING_DONE:
						currentstate = WAIT_EVENT;
						break;
					case COLOR_SENSOR_TRIGGERED:
						break;
					default:
						debug(1, "event error");
						break;
				}
				break;
			case TURN_CW:
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
			case STALL:
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
			case END:
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
}


#ifdef FSM_TEST

#include <stdio.h>
void debug(int line, char* str){
	printf("%d, %s\n", line, str);
}


static int trackStage = -1;

int RECV(){
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
			debug(1, "asked for memory incorrectly");
			event = COLOR_SENSOR_TRIGGERED;
			break;
		default:
			event = 0;
			break;
	}
	return event;
}

void moveForward(){
	printf("moving forward");
}

void turnCCW(){
	printf("turning ccw (left)");
}

void turnCW(){
	printf("turning cw (right)");
}

void stop(){
	printf("stopping rover");
}

void startTimer(){
	printf("starting timer");
}

void stopTimer(){
	printf("stopping timer");
}

void getMemory(Memory* mem){
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
	printf("hi");
	pathFindingFSM();
	return 0;
}




#endif