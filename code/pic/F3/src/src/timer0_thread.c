#include "maindefs.h"
#include <stdio.h>
#include "timer0_thread.h"

// This is a "logical" thread that processes messages from TIMER0
// It is not a "real" thread because there is only the single main thread
// of execution on the PIC because we are not using an RTOS.

int timer0_lthread(timer0_thread_struct *tptr, int msgtype, int length, unsigned char *msgbuffer) {
    unsigned int *msgval;

    msgval = (unsigned int *) msgbuffer;

    // Here is where we would do something with the message

}