#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* include files. */
#include "GLCD.h"
#include "vtUtilities.h"
#include "LCDtask.h"
#include "string.h"

// I have set this to a larger stack size because of (a) using printf() and (b) the depth of function calls
//   for some of the LCD operations
// I actually monitor the stack size in the code to check to make sure I'm not too close to overflowing the stack
//   This monitoring takes place if INPSECT_STACK is defined (search this file for INSPECT_STACK to see the code for this) 
#define INSPECT_STACK 1
#define baseStack 3
#if PRINTF_VERSION == 1
#define lcdSTACK_SIZE		((baseStack+5)*configMINIMAL_STACK_SIZE)
#else
#define lcdSTACK_SIZE		(baseStack*configMINIMAL_STACK_SIZE)
#endif

// definitions and data structures that are private to this file
// Length of the queue to this task
#define vtLCDQLen 10 
// a timer message -- not to be printed
#define LCDMsgTypeTimer 1
// a message to be printed
#define LCDMsgTypePrint 2
// actual data structure that is sent in a message
typedef struct __vtLCDMsg {
	uint8_t msgType;
	uint8_t	length;	 // Length of the message to be printed
	uint8_t buf[vtLCDMaxLen+1]; // On the way in, message to be sent, on the way out, message received (if any)
} vtLCDMsg;
// end of defs

/* definition for the LCD task. */
static portTASK_FUNCTION_PROTO( vLCDUpdateTask, pvParameters );

/*-----------------------------------------------------------*/

void StartLCDTask(vtLCDStruct *ptr, unsigned portBASE_TYPE uxPriority)
{
	if (ptr == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}

	// Create the queue that will be used to talk to this task
	if ((ptr->inQ = xQueueCreate(vtLCDQLen,sizeof(vtLCDMsg))) == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	/* Start the task */
	portBASE_TYPE retval;
	if ((retval = xTaskCreate( vLCDUpdateTask, ( signed char * ) "LCD", lcdSTACK_SIZE, (void*)ptr, uxPriority, ( xTaskHandle * ) NULL )) != pdPASS) {
		VT_HANDLE_FATAL_ERROR(retval);
	}
}

portBASE_TYPE SendLCDTimerMsg(vtLCDStruct *lcdData,portTickType ticksElapsed,portTickType ticksToBlock)
{
	if (lcdData == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	vtLCDMsg lcdBuffer;
	lcdBuffer.length = sizeof(ticksElapsed);
	if (lcdBuffer.length > vtLCDMaxLen) {
		// no room for this message
		VT_HANDLE_FATAL_ERROR(lcdBuffer.length);
	}
	memcpy(lcdBuffer.buf,(char *)&ticksElapsed,sizeof(ticksElapsed));
	lcdBuffer.msgType = LCDMsgTypeTimer;
	return(xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),ticksToBlock));
}

portBASE_TYPE SendLCDPrintMsg(vtLCDStruct *lcdData,int length,char *pString,portTickType ticksToBlock)
{
	if (lcdData == NULL) {
		VT_HANDLE_FATAL_ERROR(0);
	}
	vtLCDMsg lcdBuffer;

	if (length > vtLCDMaxLen) {
		// no room for this message
		VT_HANDLE_FATAL_ERROR(lcdBuffer.length);
	}
	lcdBuffer.length = strnlen(pString,vtLCDMaxLen);
	lcdBuffer.msgType = LCDMsgTypePrint;
	strncpy((char *)lcdBuffer.buf,pString,vtLCDMaxLen);
	return(xQueueSend(lcdData->inQ,(void *) (&lcdBuffer),ticksToBlock));
}

// Private routines used to unpack the message buffers
//   I do not want to access the message buffer data structures outside of these routines
portTickType unpackTimerMsg(vtLCDMsg *lcdBuffer)
{
	portTickType *ptr = (portTickType *) lcdBuffer->buf;
	return(*ptr);
}

int getMsgType(vtLCDMsg *lcdBuffer)
{
	return(lcdBuffer->msgType);
} 

int getMsgLength(vtLCDMsg *lcdBuffer)
{
	return(lcdBuffer->msgType);
}

void copyMsgString(char *target,vtLCDMsg *lcdBuffer,int targetMaxLen)
{
	strncpy(target,(char *)(lcdBuffer->buf),targetMaxLen);
}

static unsigned short hsl2rgb(float H,float S,float L);

// Buffer in which to store the memory read from the LCD
#define MAX_RADIUS 15
#define BUF_LEN (((MAX_RADIUS*2)+1)*((MAX_RADIUS*2)+1))
static unsigned short int buffer[BUF_LEN];

// This is the actual task that is run
static portTASK_FUNCTION( vLCDUpdateTask, pvParameters )
{
	int xoffset = 0, yoffset = 0;
	unsigned int xmin=0, xmax=0, ymin=0, ymax=0;
	unsigned int x, y;
	int i, j;
	float hue=0, sat=0.2, light=0.2;
	unsigned char curLine = 5;
	
	vtLCDMsg msgBuffer;
	vtLCDStruct *lcdPtr = (vtLCDStruct *) pvParameters;

	/* Initialize the LCD and set the initial colors */
	GLCD_Init();
	unsigned short tscr = Red; // may be reset in the LCDMsgTypeTimer code below
	unsigned short screenColor = Green; // may be reset in the LCDMsgTypeTimer code below
	GLCD_SetTextColor(tscr);
	GLCD_SetBackColor(screenColor);
	GLCD_Clear(screenColor);

	// This task should never exit
	for(;;)
	{
		// Wait for a message
		if (xQueueReceive(lcdPtr->inQ,(void *) &msgBuffer,portMAX_DELAY) != pdTRUE) {
			VT_HANDLE_FATAL_ERROR(0);
		}
		
		//Log that we are processing a message -- more explanation of logging is given later on
		vtITMu8(vtITMPortLCDMsg,getMsgType(&msgBuffer));
		vtITMu8(vtITMPortLCDMsg,getMsgLength(&msgBuffer));

		// Take a different action depending on the type of the message that we received
		switch(getMsgType(&msgBuffer)) {
		case LCDMsgTypePrint: {
			// This will result in the text printing in the last five lines of the screen
			char   lineBuffer[lcdCHAR_IN_LINE+1];
			copyMsgString(lineBuffer,&msgBuffer,lcdCHAR_IN_LINE);
			// clear the line
			GLCD_ClearLn(curLine,1);
			// show the text
			GLCD_DisplayString(curLine,0,1,(unsigned char *)lineBuffer);
			curLine++;
			if (curLine == lcdNUM_LINES) {
				curLine = 3;
			}
			break;
		}
		case LCDMsgTypeTimer:
			break;
		default: {
			// In this configuration, we are only expecting to receive timer messages
			VT_HANDLE_FATAL_ERROR(getMsgType(&msgBuffer));
			break;
		}
		} // end of switch()

		// Here is a way to do debugging output via the built-in hardware -- it requires the ULINK cable and the
		//   debugger in the Keil tools to be connected.  You can view PORT0 output in the "Debug(printf) Viewer"
		//   under "View->Serial Windows".  You have to enable "Trace" and "Port0" in the Debug setup options.  This
		//   should not be used if you are using Port0 for printf()
		// There are 31 other ports and their output (and port 0's) can be seen in the "View->Trace->Records"
		//   windows.  You have to enable the prots in the Debug setup options.  Note that unlike ITM_SendChar()
		//   this "raw" port write is not blocking.  That means it can overrun the capability of the system to record
		//   the trace events if you go too quickly; that won't hurt anything or change the program execution and
		//   you can tell if it happens because the "View->Trace->Records" window will show there was an overrun.
		//vtITMu16(vtITMPortLCD,screenColor);
	}
}

// Convert from HSL colormap to RGB values in this weird colormap
// H: 0 to 360
// S: 0 to 1
// L: 0 to 1
// The LCD has a funky bitmap.  Each pixel is 16 bits (a "short unsigned int")
//   Red is the most significant 5 bits
//   Blue is the least significant 5 bits
//   Green is the middle 6 bits
static unsigned short hsl2rgb(float H,float S,float L)
{
	float C = (1.0 - fabs(2.0*L-1.0))*S;
	float Hprime = H / 60;
	unsigned short t = Hprime / 2.0;
	t *= 2;
	float X = C * (1-abs((Hprime - t) - 1));
	unsigned short truncHprime = Hprime;
	float R1, G1, B1;

	switch(truncHprime) {
		case 0: {
			R1 = C; G1 = X; B1 = 0;
			break;
		}
		case 1: {
			R1 = X; G1 = C; B1 = 0;
			break;
		}
		case 2: {
			R1 = 0; G1 = C; B1 = X;
			break;
		}
		case 3: {
			R1 = 0; G1 = X; B1 = C;
			break;
		}
		case 4: {
			R1 = X; G1 = 0; B1 = C;
			break;
		}
		case 5: {
			R1 = C; G1 = 0; B1 = X;
			break;
		}
		default: {
			// make the compiler stop generating warnings
			R1 = 0; G1 = 0; B1 = 0;
			VT_HANDLE_FATAL_ERROR(Hprime);
			break;
		}
	}
	float m = L - 0.5*C;
	R1 += m; G1 += m; B1 += m;
	unsigned short red = R1*32; if (red > 31) red = 31;
	unsigned short green = G1*64; if (green > 63) green = 63;
	unsigned short blue = B1*32; if (blue > 31) blue = 31;
	unsigned short color = (red << 11) | (green << 5) | blue;
	return(color); 
}
