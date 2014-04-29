#ifndef FSM_H_INC
#define FSM_H_INC

//#define FSM_TEST


void pathFindingFSM();

typedef enum {
	NEW_SENSOR_DATA = 1,
	TURN_COMPLETE = 2,
	TICK_COUNTING_DONE = 3,
	COLOR_SENSOR_TRIGGERED = 4,
	RESET_ROVER = 5,
	START = 6,
} FsmEvent;


#define INIT 0
#define WAIT_START_LINE 1 //also continues to move forward
#define WAIT_EVENT 2
#define WAIT_EVENT_SLOW 10
#define WAIT_TURN 11
#define L_TURN_STALL 3
#define R_TURN_STALL 4
#define ADJ_TURN_STALL 8
#define WAIT_TICKS 5
#define END 7 //picman handles stop frame data and stop moving

#ifndef FSM_TEST
// Safe to be called on any task
void TriggerEvent(FsmEvent);
#endif



#endif //FSM_H_INC
