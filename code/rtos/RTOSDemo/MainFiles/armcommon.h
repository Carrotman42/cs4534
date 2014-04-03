
// Macros and functions to make programming with tasks more convenient and organized.
#ifndef COMMON_H_INC
#define COMMON_H_INC

#define PICMAN_I2C_ADDR 0x10
#define ARM
// Define this for debug checks that double-check the programmer. Will be disabled for final run once testing is over.
#define CHECKS
#define ETHER_EMU 1

// Common includes:
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Our local types
#include "types.h"
// Helpers given to us
#include "vtUtilities.h"
// Helpers I wrote
#include "kdbg.h"

#define _m2S(x) #x
#define m2S(x) _m2S(x)

#ifdef CHECKS
// Note: This won't work if the LCD fails, so if you think that could be happening
//    then take out the LCDwriteLn
// TODO: Include line number too.
#define FATAL(x) FATALSTR("FAIL: " __FILE__ ":" m2S(__LINE__))
#else
#define FATAL(x) VT_HANDLE_FATAL_ERROR(x)
#endif

#define FATALSTR(str) { \
		/* Only write to the LCD if the scheduler has started */ \
		if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) { \
			void LCDwriteLn(int line, char* data); \
			LCDwriteLn(8, str); \
			/* Hacky way to help make sure the LCD will draw this */ \
			vTaskDelay(1000/portTICK_RATE_MS); \
		} \
		VT_HANDLE_FATAL_ERROR(0); \
	}

// This macro helps with checking return codes
#define FAILIF(x) if ((x)) FATAL(0);

#define SLEEP(ms) vTaskDelay(ms/portTICK_RATE_MS)

#ifdef CHECKS
#ifndef ASSERT
#define ASSERT(x) if (!(x)) FATAL(0);
#endif
#else
#define ASSERT(x)
#endif

// These macros have to do with queues and are relatively straightforward.
#define MAKE_Q(dest, type, len) if (((dest) = xQueueCreate(len, sizeof(type))) == NULL) FATAL(0);
#define RECV(chan, dest) if(xQueueReceive(chan,(void *)&(dest),portMAX_DELAY) != pdTRUE) FATAL(0);
#define TRY_RECV(chan, dest) (xQueueReceive(chan,(void *)&(dest), 0) == pdTRUE)
#define SEND(chan, src)  if(xQueueSend(chan, &src, portMAX_DELAY) != pdTRUE) FATAL(0);

// Helper defines, used internally in this common.h. They are considered private and should not
//    be used outside of this file.
#define __stip(name, inside, period) \
	unsigned short const name##PERIOD = period; \
	xTimerHandle Make##name(inside)
#define __stap(name, inside, stackSize, priority) \
	unsigned short const name##STACK_SIZE = stackSize; \
	unsigned portBASE_TYPE const name##PRIORITY = priority; \
	void Start##name(inside)

// Use this to declare a timer callback prototype (usually in a header file)
#define TIMER_PROTOTYPE(name, type, period) __stip(name, type *ps, period)
#define TIMER_PROTOTYPE_NOARG(name, period) __stip(name,     void, period)

// Use this to declare a task (usually in a header file)
#define TASK_PROTOTYPE(name, type, stackSize, priority) __stap(name, type* ps, stackSize, priority);
#define TASK_PROTOTYPE_NOARG(name, stackSize, priority) __stap(name,     void, stackSize, priority);

#define __stif(name, inside, createParam) \
	static void name(xTimerHandle pxTimer); \
	xTimerHandle Make##name(inside) { \
		xTimerHandle retval; \
		FAILIF((retval = xTimerCreate((const signed char*) #name, name##PERIOD, pdTRUE, createParam, name)) == NULL); \
		return retval; \
	} \
	static void name(xTimerHandle pxTimer) {
#define __staf(name, inside, createParam, funcParamName) \
	static portTASK_FUNCTION(name, orig_##funcParamName); \
	void Start##name(inside) { \
		portBASE_TYPE retval; \
		if ((retval = xTaskCreate(name, (signed char*) #name, name##STACK_SIZE, createParam, name##PRIORITY, NULL )) != pdPASS) { \
			FATAL(retval); \
		} \
	} \
	static portTASK_FUNCTION(name, orig_##funcParamName) {

// Use this to define a timer (usually in a source (.c) file)
#define TIMER_FUNC(name, type, paramName) __stif(name, type* ps, ps) \
											DOESN'T WORK yet; '\
											type* paramName = (type*)orig_##paramName;
#define TIMER_FUNC_NOARG(name)            __stif(name, void, NULL);
#define ENDTIMER }
// Use this to define a task (usually in a source (.c) file)
// NOTE: You need an asscociated TASK_PROTOTYPE for every TASK_FUNC, (yes, even if it's in the same file).
//   But Usually the TASK_FUNC goes in a .c file and the TASK_PROTOTYPE goes in a .h file.
#define TASK_FUNC(name, type, paramName) __staf(name, type* ps, ps, paramName)	\
                                         	type* paramName = (type*)orig_##paramName;
#define TASK_FUNC_NOARG(name)            __staf(name, void, NULL, unusedParam)
#define ENDTASK }


#define START_TIMER(handle, blockticks) FAILIF(xTimerStart(handle,blockticks) != pdPASS)

#define INSPECT_STACK(max_stack) \
	unsigned portBASE_TYPE InitialStackLeft = uxTaskGetStackHighWaterMark(NULL); \
	unsigned portBASE_TYPE CurrentStackLeft;\
	float remainingStack = InitialStackLeft;\
	remainingStack /= max_stack;\
	if (remainingStack < 0.10) {\
		FATAL(0); \
	}


// staticf (Static string format) functions
#define aBuf(name, len) char name##name[len], *name = name##name;

#define aStr(dest, str) {   \
		char* s = str, c;	\
		while ((c = *s)) {	\
		 	aChar(dest, c);	\
			s++;			\
		}					\
	}

#define aWord(dest, w) \
	aByte(dest, (w >> 8) & 0xFF);\
	aByte(dest, w & 0xFF);

#define aByte(dest, val) {         \
		int t = (val);             \
		aNib(dest, (t / 16) % 16); \
		aNib(dest, (t     ) % 16); \
	}

#define aNib(dest, val) \
	aChar(dest, (val < 10) ? '0' + val : 'A' + val - 10)

#define aChar(dest, ch) *dest++ = (ch)
// Also resets the pointer to the beginning of the buffer so that it
//   can be reused easily
#define aPrint(name, line) LCDwriteLn(line, name##name); name = name##name

// Convenience functions for naming the buffer 'b'. Also don't require you to a char(0) before printing
#define bBuf(p)   aBuf(b, p)
#define bStr(p)   aStr(b, p)
#define bByte(p)  aByte(b, p)
#define bWord(p)  aWord(b, p)
#define bNib(p)   aNib(b, p)
#define bChar(p)  aChar(b, p)
#define bPrint(p) aChar(b, 0); aPrint(b, p)

#endif