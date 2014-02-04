
// Macros and functions to make programming with tasks more convenient and organized.
#ifndef COMMON_H_INC
#define COMMON_H_INC

// Define this for debug checks that double-check the programmer. Will be disabled for final run once testing is over.
#define CHECKS

// Common includes:
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Our local types
#include "types.h"

// Could swap this out with a print to LCD (or log file on the SD card) the __FILE__ and __LINE__ in debug mode
#define FATAL(x) VT_HANDLE_FATAL_ERROR(x);

// This macro helps with checking return codes
#define FAILIF(x) if (x) FATAL(0);

// These macros have to do with queues and are relatively straightforward.
#define MAKE_Q(dest, type, len) if (((dest) = xQueueCreate(len, sizeof(type))) == NULL) FATAL(0);
#define RECV(chan, dest) if(xQueueReceive(chan,(void *)&(dest),portMAX_DELAY) != pdTRUE) FATAL(0);
#define SEND(chan, src)  if(xQueueSend(chan, &src, portMAX_DELAY) != pdTRUE) FATAL(0);


// Use this to define a task (usually in a header file)
#define TASK_PROTOTYPE(name, type, stackSize, priority) \
	unsigned short const name##STACK_SIZE = stackSize; \
	unsigned portBASE_TYPE const name##PRIORITY = priority; \
	void Start##name(type* ps);

// Use this to declare a task (usually in a source (.c) file)
// NOTE: You need an asscociated TASK_PROTOTYPE for every TASK_FUNC, (yes, even if it's in the same file).
//   But Usually the TASK_FUNC goes in a .c file and the TASK_PROTOTYPE goes in a .h file.
#define TASK_FUNC(name, type, paramName) \
	static portTASK_FUNCTION(name, orig_##paramName); \
	void Start##name(type* ps) { \
		portBASE_TYPE retval; \
		if ((retval = xTaskCreate(name, (signed char*) #name, name##STACK_SIZE, ps, name##PRIORITY, NULL )) != pdPASS) { \
			FATAL(retval); \
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
		FATAL(0); \
	}

#endif