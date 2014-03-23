#ifndef FSM_H_INC
#define FSM_H_INC

#define FSM_TEST

#include "map.h"


void pathFindingFSM();

#ifdef FSM_TEST

void debug(int line, char* str);
int RECV();
void moveForward();
void turnCCW();
void turnCW();
void stop();
void startTimer();
void stopTimer();
void getMemory(Memory* mem);
#endif

#endif //FSM_H_INC
