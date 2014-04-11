#include "maindefs.h"
#include "my_ultrasonic.h"
#include "messages.h"
#include "debug.h"
#include "my_uart.h"


static char timerHigh = 0,timerLow = 0;
static unsigned char timer2Data = 0;
//static char data[2];

//Sets up RB0 as External Interrupt and configures Timer for counting time elapsed
void initUS(){
    //Configure Interrupts for RB0
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 1;
    TRISBbits.RB0 = 1;

    //Configure Timer2
#ifdef SENSOR_PIC
//    T2CON = 0x00;  //Postscale = 1:1, Prescale = 1:1, TMR2 = OFF
//    TMR2 = 0; //Clear Timer2
//    PIR1bits.TMR2IF = 0;  // Clear int flag
//    PIE1bits.TMR2IE = 1;  // Enable int
//
//    IPR1bits.TMR2IP = 1;  // High priority
//    PR2 = 0x00;
#endif

    timer2Data = 0;
}

void pulseUS(){
    //Pulse Signal line
    TRISBbits.TRISB0 = 0;
//    LATBbits.LATB0 = 1;
//    LATBbits.LATB0 = 0;
    debugNum(1);
    TRISBbits.TRISB0 = 1;
//    debugNum(8)

    //Enable edge selection interrupt
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 1;
}
void startTimerUS(){
    TMR2 = 0; //Clears Timer2
    T2CONbits.TMR2ON = 1;  //Turn on Timer 2
}

void stopTimerUS(){
    T2CONbits.TMR2ON = 0;  //Turn off Timer 2
}

//char* getDistanceUS(){
//    return data;
//}

void us_int_handler(){
    //Start of echo pulse
    debugNum(2);
    if(INTCON2bits.INTEDG0 == 1){
        startTimerUS();
        INTCON2bits.INTEDG0 = 0;
    }
    else if(INTCON2bits.INTEDG0 == 0){
        stopTimerUS();
//        timerHigh = TMR1H;
//        timerLow = TMR1L;
//        TMR1H = 0xFF;
//        TMR1L = 0x6D;
        timer2Data = TMR2;
        INTCON2bits.INTEDG0 = 1;

        uart_send(timer2Data);

//        char data[2];
//        data[0] = timerHigh;
//        data[1] = timerLow;
//        uart_send_array(data,2);
    }    
}