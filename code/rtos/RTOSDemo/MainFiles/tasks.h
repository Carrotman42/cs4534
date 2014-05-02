
// This is a collection of all of the tasks that could be run by the program. It is included from common.h .
//    The point of keeping them all here is so that all the priorities and stack spaces can be kept nearby.

// Because different tasks may have strange parameters that are made of types that are private, I use a preprocessor
//    approach to only request the task prototypes if you want them. If you want to use a certain task, before you include
//    this file you should #define all of the tasks you want included.

#ifndef TASKS_H_INC
#define TASKS_H_INC

#ifdef LCD_TASKS
// The task to display a signal on the LCD screen
TASK_PROTOTYPE(LCDTask, LCDBuf, 1000, tskIDLE_PRIORITY);
TASK_PROTOTYPE_NOARG(TestSignalTask, 200, tskIDLE_PRIORITY);
#endif

#ifdef PERIODIC_TASKS
TASK_PROTOTYPE_NOARG(PeriodicTask, 500, tskIDLE_PRIORITY);
#endif

#ifdef SENSORPROC_TASKS
TASK_PROTOTYPE_NOARG(ProcessEvents, 2000, tskIDLE_PRIORITY);
#endif

#ifdef I2C_TASKS
#define mainI2CMONITOR_TASK_PRIORITY		( tskIDLE_PRIORITY+2)
#define I2C_Stack 50000
#endif

#ifdef FSM_TASKS
TASK_PROTOTYPE_NOARG(PathFindingFSM, 1500, tskIDLE_PRIORITY+1);
#endif

#endif

#define mainUIP_TASK_PRIORITY				( tskIDLE_PRIORITY+2)

