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

#ifndef FSM_TEST
// Safe to be called on any task
void TriggerEvent(FsmEvent);
#endif



#endif //FSM_H_INC
