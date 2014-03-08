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

#ifdef MASTER_PIC
    debugNum(1);
    char encoderDataReq[5];
    //uint8 length = generateGetEncoderData(encoderDataReq, sizeof encoderDataReq);
    uint8 length = generateGetSensorFrame(encoderDataReq, sizeof encoderDataReq);
    //uart_send_array(encoderDataReq, length);
    i2c_master_send(MOTOR_ADDR, length, encoderDataReq);
    WriteTimer0(0x4000);
#endif


#ifdef MASTER_PIC
#ifdef DEBUG_ON
    //WriteTimer0(0x4000);
    //debugNum(4);
    //i2c_master_recv(0x10);
    //char buf[1];
    //buf[0] = 0x01;
    //i2c_master_send(0x10, 1, buf);
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
    #if GLEN_DEBUG == 1
    static char temp = 0;
//    if (temp++ == 0) {
//        unsigned char test[5] = {'1','2','3','4','\r'};
        unsigned char test[5] = {1,0,0,1,0};
        uart_send_array(&test, 5);
        
//    } else {
//        unsigned char test[5] = {'1','3','2','4','\r'};
//        uart_send_array(&test, 5);
//        temp = 0;
//    }
#endif
    //unsigned char test[5] = {0x01,0x0,0x0,0x01,0x0};
    //ToMainLow_sendmsg(5, MSGT_UART_DATA, (void*) test);

    // reset the timer
    //WriteTimer1(0xFFFF-3750);
#if defined(MASTER_PIC) && defined(DEBUG_ON)
    WriteTimer1(0x4000);
    //i2c_master_recv(0x10);
#endif

#if defined(PICMAN) && defined(DEBUG_ON)
    WriteTimer1(0x4000);
    //i2c_master_recv(0x10);

    /*static char temp = 0;
    if (temp++ == 0) {
        unsigned char test[5] = {0x01,0x0,0x0,0x0,0x01};
        uart_send_array(test, 5);
    //ToMainHigh_sendmsg(5, MSGT_I2C_DATA, (void *) test);
    }else {*/
        unsigned char test[7] = {0x02, 0x4, 0x0, 0x02, 0x9, 0x1, 0x0};
        //unsigned char test[5] = {0x02, 0x5, 0x0, 0x00, 0x7};
        uart_send_array(&test, sizeof test);
        //temp = 0;
    //}
#elif defined(MASTER_PIC) && defined(DEBUG_ON)
        //debugNum(2);
        static uint8 temp =0;
        char testArray[6];
        uint8 length = 0;
        switch(temp){
            case 0:
                length = generateStartForward(testArray, sizeof testArray, I2C_COMM, 0x05);
                temp++;
                break;
            case 1:
                length = generateStartBackward(testArray, sizeof testArray, I2C_COMM, 0x06);
                temp++;
                break;
            case 2:
                length = generateStop(testArray, sizeof testArray, I2C_COMM);
                temp++;
                break;
            case 3:
                length = generateTurnCW(testArray, sizeof testArray, I2C_COMM, 0x07);
                temp++;
                break;
            case 4:
                length = generateTurnCCW(testArray, sizeof testArray, I2C_COMM, 0x08);
                temp++;
                break;
            case 5:
                temp = 0;
                break;
        }

        //uart_send_array(testArray, length);
        ToMainLow_sendmsg(length, MSGT_UART_DATA, (void*) testArray);
        //i2c_master_send(MOTOR_ADDR, length, (char *) frameReq);
        WriteTimer1(0x4000);
#endif

}