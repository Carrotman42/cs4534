#include "maindefs.h"
#include "my_ultrasonic.h"
#include "messages.h"
#include "debug.h"
#include "my_uart.h"


static char timerHigh = 0,timerLow = 0;
//static char data[2];

//Sets up RB0 as External Interrupt and configures Timer for counting time elapsed
void initUS(){
    //Configure Interrupts for RB0
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 1;
    TRISBbits.RB0 = 1;

    //Configure Timer1
//    OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT &T1_PS_1_1);
    T1CONbits.RD16 = 1;
    T1CONbits.T1CKPS = 0;
}

void pulseUS(){
    //Pulse Signal line
    TRISBbits.RB0 = 0;
    LATBbits.LB0 = 1;
    LATBbits.LB0 = 0;
    TRISBbits.RB0 = 1;
    debugNum(8);

    //Enable edge selection interrupt
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 1;
}
void startTimerUS(){
    TMR1H = 0x00;
    TMR1L = 0x00;
    T1CONbits.TMR1ON = 1;
}

void stopTimerUS(){
    T1CONbits.TMR1ON = 0;
}

//char* getDistanceUS(){
//    return data;
//}

void us_int_handler(){
    //Start of echo pulse
    if(INTCON2bits.INTEDG0 == 1){
        startTimerUS();
        INTCON2bits.INTEDG0 = 0;
    }
    else if(INTCON2bits.INTEDG0 == 0){
        stopTimerUS();
        timerHigh = TMR1H;
        timerLow = TMR1L;
        TMR1H = 0xFF;
        TMR1L = 0x6D;
        INTCON2bits.INTEDG0 = 1;

        char data[2];
        data[0] = timerHigh;
        data[1] = timerLow;
//        uart_send_array(data,2);
    }    
}