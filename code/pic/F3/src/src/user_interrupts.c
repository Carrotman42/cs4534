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
    //debugNum(2);
    static uint8 loop = 0;

    char data[5];
    uint8 length = 0;
    uint8 addr;
    switch(loop){
        case 0:
            length = generateGetSensorFrame(data, sizeof data);
            loop++;
            addr = SENSOR_ADDR;
            break;
        case 1:
            length = generateGetEncoderData(data, sizeof data);
            loop = 0;
            addr = MOTOR_ADDR;
            break;
        default:
            loop = 0;
            break;
    }
    //uart_send_array(encoderDataReq, length);
    i2c_master_send(addr, length, data);
    WriteTimer0(0x2000);
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

#if defined(ARM_EMU) && defined(DEBUG_ON)
        static uint8 temp =0;
        static uint8 start = 0;
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
                if(start == 0){
                    length = generateStartFrames(testArray, sizeof testArray, I2C_COMM);
                    start++;
                }
                else if(start == 1){
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    start++;
                }
                else{
                    length = generateStopFrames(testArray, sizeof testArray, I2C_COMM);
                    start = 0;
                }

                temp = 0;
                break;
        }
        i2c_master_send(PICMAN_ADDR, length, (char *) testArray);
        WriteTimer1(0x4000);
#elif defined(MASTER_PIC) && defined(DEBUG_ON)
        //debugNum(1);
        static uint8 temp =0;
        static uint8 start = 0;
        char testArray[6];
        uint8 length = 0;
        if(isTurnComplete()){ //make sure rover is not turning
            switch(temp){
                case 0:
                    length = generateStartForward(testArray, sizeof testArray, UART_COMM, 0x05);
                    temp++;
                    break;
                case 1:
                    length = generateStartBackward(testArray, sizeof testArray, UART_COMM, 0x06);
                    temp++;
                    break;
                case 2:
                    length = generateStop(testArray, sizeof testArray, UART_COMM);
                    temp++;
                    break;
                case 3:
                    length = generateTurnCW(testArray, sizeof testArray, UART_COMM, 0x07);
                    temp++;
                    break;
                case 4:
                    length = generateTurnCCW(testArray, sizeof testArray, UART_COMM, 0x08);
                    temp++;
                    break;
                case 5:
                    if(start == 0){
                        length = generateStartFrames(testArray, sizeof testArray, UART_COMM);
                        start = 1;
                    }
                    else{
                        length = generateStopFrames(testArray, sizeof testArray, UART_COMM);
                        start = 0;
                    }

                    temp = 0;
                    break;
            }


            //uart_send_array(testArray, length);
            ToMainLow_sendmsg(length, MSGT_UART_DATA, (void*) testArray);
        }
        //i2c_master_send(MOTOR_ADDR, length, (char *) frameReq);
        //WriteTimer1(0x4000);
#endif

}