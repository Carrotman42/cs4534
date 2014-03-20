#ifndef SENSORPROC_H
#define SENSORPROC_H


typedef enum {
	NewFrame = 1,
	TurnAck = 2,
} Event;

#define SENSORPROC_TASKS
#include "tasks.h"

#endif