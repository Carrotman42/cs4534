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
#include <stdbool.h>
#include "motor.h"
#include "interrupts.h"


// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt
 int counter = 0;
 // int motor0Ticks;
 // int motor1Ticks;

void timer0_int_handler() {

#ifdef MASTER_PIC
#ifdef DEBUG_ON
    WriteTimer0(0x4000);
    //debugNum(4);
    //i2c_master_recv(0x10);
    char buf[1];
    buf[0] = 0x01;
    i2c_master_send(0x10, 1, buf);
#endif
#endif

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
    //debugNum(4);
    //uart_send((char) 0x55);
//    result = ReadTimer1();


    /* Test UART send, send one correct sequence and one incorrect
     The end device will test based on the correct sequence and turn a led on
     or off based on what was recieved*/
//    static char temp = 0;
//    if (temp++ == 0) {
//        unsigned char test[5] = {'1','2','3','4','\r'};
//        uart_send_array(&test, 5);
//    } else {
//        unsigned char test[5] = {'1','3','2','4','\r'};
//        uart_send_array(&test, 5);
//        temp = 0;
//    }

    //  test for dry run
    counter++;
    // 6 seconds of forwards
    if (counter < 50)
    {
        forward();
    }
    // 6 seconds of reverse
    else if ( (counter > 50) && (counter < 100) )
    {
        reverse();
    }
    // 6 seconds motor 1 forwards, motor 2 stops
    else if ( (counter > 100) && (counter < 150) )
    {
        forwardMotor1();
    }
    // 6 seconds motor 2 forwards, motor 1 stops
    else if ( (counter > 150) && (counter < 200) )
    {
        forwardMotor2();
    }
    // stop both motors
    else
    {
        stop();
    }


    // pass motor ticks from main
    // TODO: oldMotor0Ticks = &passedMotor0Ticks;
    // TODO: oldMotor1Ticks = &passedMotor1Ticks;
   // TODO: passedMotorTicks = 0;   // reset motor ticks for the new updates later
    oldMotor0Ticks = motor0Ticks;
    oldMotor1Ticks = motor1Ticks;
    motor0Ticks = 0;
    motor1Ticks = 0;


    //ToMainLow_sendmsg(0, MSGT_UART_DATA, (void*) 0);

    // reset the timer
    //WriteTimer1(0xFFFF-3750);
#if defined(MASTER_PIC) && defined(DEBUG_ON)
    WriteTimer1(0x4000);
    i2c_master_recv(0x10);
#endif
}