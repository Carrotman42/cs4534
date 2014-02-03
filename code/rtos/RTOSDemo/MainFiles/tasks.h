
#ifndef TASKS_H_INC
#define TASKS_H_INC

// Macros to make programming with tasks more convenient and organized

#define MAKE_Q(dest, type, len) if (((dest) = xQueueCreate(len, sizeof(type))) == NULL) VT_HANDLE_FATAL_ERROR(0);
#define RECV(chan, dest) if(xQueueReceive(chan,(void *)&(dest),portMAX_DELAY) != pdTRUE) VT_HANDLE_FATAL_ERROR(0);
#define SEND(chan, src)  if(xQueueSend(chan, &src, portMAX_DELAY) != pdTRUE) VT_HANDLE_FATAL_ERROR(0);

#define TASK_PROTOTYPE(name, type, stackSize, priority) \
	unsigned short const name##STACK_SIZE = stackSize; \
	unsigned portBASE_TYPE const name##PRIORITY = priority; \
	void Start##name(type* ps);

// NOTE: You need an asscociated TASK_PROTOTYPE for every TASK_FUNC. The TASK_FUNC goes in a .c file and the TASK_PROTOTYPE goes
//   in a .h file.
#define TASK_FUNC(name, type, paramName) \
	static portTASK_FUNCTION(name, orig_##paramName); \
	void Start##name(type* ps) { \
		portBASE_TYPE retval; \
		if ((retval = xTaskCreate(name, (signed char*) #name, name##STACK_SIZE, ps, name##PRIORITY, NULL )) != pdPASS) { \
			VT_HANDLE_FATAL_ERROR(retval); \
		} \
	} \
	static portTASK_FUNCTION(name, orig_##paramName) { \
		type* paramName = (type*)orig_##paramName;

#define ENDTASK }

#define INSPECT_STACK(max_stack) \
	unsigned portBASE_TYPE InitialStackLeft = uxTaskGetStackHighWaterMark(NULL); \
	unsigned portBASE_TYPE CurrentStackLeft;\
	float remainingStack = InitialStackLeft;\
	remainingStack /= max_stack;\
	if (remainingStack < 0.10) {\
		VT_HANDLE_FATAL_ERROR(0); \
	}



#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "types.h"
// All of the public tasks should be prototyped here:

void StartSignalTest();




#endif