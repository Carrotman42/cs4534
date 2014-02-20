// This is where the "user" interrupts handlers should go
// The *must* be declared in "user_interrupts.h"

#include "maindefs.h"
#ifndef __XC8
#include <timers.h>
#else
#include <plib/timers.h>
#endif
#include "user_interrupts.h"
#include "messages.h"
#include "debug.h"

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt

void timer0_int_handler() {

<<<<<<< HEAD
#ifdef MASTER_PIC
#ifdef DEBUG_ON
    WriteTimer0(0x4000);
    //debugNum(8);
    i2c_master_recv(0x4f);
    //char buf[2];
    //buf[0] = 0xaa;
    //buf[1] = 0xbb;
    //i2c_master_send(0x4F, 2, buf);
#endif
#endif

=======
>>>>>>> 5f00084c4622bbb06b6acf10d71f96fb2eab59cf
#ifdef SENSOR_PIC
    //ADCON0bits.GO = 1;
    //WriteTimer0(0xFFFF-375);
#endif //SENSOR_PIC
}

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer1 interrupt

void timer1_int_handler() {
//    unsigned int result;
    // read the timer and then send an empty message to main()
#ifdef __USE18F2680
    LATBbits.LATB1 = !LATBbits.LATB1;
#endif
    debugNum(4);
    uart_send((char) 0x55);
//    result = ReadTimer1();
    //ToMainLow_sendmsg(0, MSGT_TIMER1, (void *) 0);

    // reset the timer
    WriteTimer1(0xFFFF-3750);
}