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
#ifdef ROVER_EMU
#include "../../../../common/communication/frames.h"
#endif

#include <stdbool.h>
#include "motor.h"
#include "interrupts.h"

#ifdef SENSOR_PIC
#include "my_adc.h"
#include "my_ultrasonic.h"
#endif


// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt

unsigned char datareq = 0;
uint16_t motor1Ticks = 0;
uint16_t motor2Ticks = 0;
uint16_t target1 = 0;
uint16_t target2 = 0;
bool commandDone = false;

// ticks sent to the ARM for arm unit calculations
int finalMotor1Ticks = 0;
int finalMotor2Ticks = 0;
bool ticks1Sent = false;
bool ticks2Sent = false;


 // motor 1 ticks for 1 revolution: 2750
 // motor 2 ticks for 1 revolution: 2675
 // target values: 1 interrupt = 25 ticks
 // moved them to motor.h file
 //int target1 = 610;     // 110 for 1 revolution
 //int target2 = 108;     // 107 for 1 revolution

void timer0_int_handler() {
#ifdef MASTER_PIC
#ifdef DEBUG_ON
    static int colorSensorCounter = 0;
    static uint8 in_progress = 0;
    if(colorSensorCounter == 100){
        colorSensorCounter = 0;
        char command[5] = {0};
        uint8 length = generateColorSensorSensed(command, sizeof command, UART_COMM);
        uart_send_array(command, length);
        in_progress++;
        debugNum(1);
    }
    else if(in_progress < 2){
        colorSensorCounter++;
    }
#endif
    static uint8 loop = 0;

    char data[5] = {0};
    uint8 length = 0;
    uint8 addr;
    static uint8 datareq_loop = 0;
    
    if(!datareq){
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
        datareq = 1;
        i2c_master_send(addr, length, data);
    }
    else{
        datareq_loop++;
        if(datareq_loop == 10){ //reset after 5
            datareq = 0;
            datareq_loop = 0;
        }
    }
    WriteTimer0(0x4000);
#endif


#ifdef MASTER_PIC
#ifdef DEBUG_ON
    //WriteTimer0(0x4000);
    //i2c_master_recv(0x10);
    //char buf[1];
    //buf[0] = 0x01;
    //i2c_master_send(0x10, 1, buf);
#endif
#endif

#ifdef SENSOR_PIC
    ADCON0bits.GO = 1;  //Start ADC sampling
    pulseUS();  //Start US Sampling
//    WriteTimer0(0xFFFF-1500);
    WriteTimer0(0xFFFF-9375+1875);
//    debugNum(8);
#endif //SENSOR_PIC

   // encoders for motor 0
#ifdef MOTOR_PIC
    motor1Ticks++;
    if (ticks1Sent)
    {
        finalMotor1Ticks = 0;
        ticks1Sent = true;
    }
    finalMotor1Ticks++;     // ticks to be sent to the ARM
    if (motor1Ticks > target1)
    {
        // stop it here and break it down to a function each and call multiple functions
        // to get the job done
        stop();
        commandDone = true;
        
        // dont use, slipping errors occur, this is used to adjust speeds of
        // each motor to be the same
        //stopMotor1();    
    }
    WRITETIMER0(0x00E8);

#endif
}

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer1 interrupt

void timer1_int_handler() {
    debugNum(4);
//    unsigned int result;
    // read the timer and then send an empty message to main()
#ifdef __USE18F2680
    LATBbits.LATB1 = !LATBbits.LATB1;
#endif
    //uart_send((char) 0x55);
//    result = ReadTimer1();
#if defined(ROVER_EMU) && defined(DEBUG_ON)
        //the only thing the rover does on its own is send data
    static uint8 colorSensorCounter = 0;
    if(colorSensorCounter == 50){
        colorSensorCounter++;
        char command[5];
        uint8 length = generateColorSensorSensed(command, sizeof command, UART_COMM);
        uart_send_array(command, length);
    }
    else if(colorSensorCounter < 50){
        fillDummyFrame();
        if(frameDataReady()){ //used for checking the framesrequested flag
            sendFrameData();
            clearFrameData();
        }
        colorSensorCounter++;
    }

    WriteTimer1(0x0000);

#elif defined(ARM_EMU) && defined(DEBUG_ON)
        static uint8 temp =0;
        if(isTurnComplete() && !isColorSensorTriggered()){
            char testArray[10];
            uint8 length = 0;
            switch(temp){
                case 0:
                    length = generateStartFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 1:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 2:
                    length = generateStartForward(testArray, sizeof testArray, I2C_COMM, 0x05);
                    temp++;
                    break;
                case 3:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 4:
                    length = generateStartBackward(testArray, sizeof testArray, I2C_COMM, 0x06);
                    temp++;
                    break;
                case 5:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 6:
                    length = generateStop(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 7:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 8:
                    length = generateTurnCW(testArray, sizeof testArray, I2C_COMM, 90);
                    temp++;
                    break;
                case 9:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 10:
                    length = generateTurnCCW(testArray, sizeof testArray, I2C_COMM, 90);
                    temp++;
                    break;
                case 11:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 12:
                    length = generateDoVictoryDance(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 13:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 14:
                    length = generateReadjustCW(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 15:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 16:
                    length = generateReadjustCCW(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 17:
                    length = generateReadFrames(testArray, sizeof testArray, I2C_COMM);
                    temp++;
                    break;
                case 18:
                    debugNum(1);
                    length = generateGoForwardDistanceTurn(testArray, sizeof testArray, I2C_COMM, 1, 20, 0);
                    temp = 0;
                    break;
            }
            i2c_master_send(PICMAN_ADDR, length, (char *) testArray);
        }
        WriteTimer1(0x2000);
#elif defined(MASTER_PIC) && defined(DEBUG_ON)
//        static uint8 temp =0;
//        static uint8 start = 0;
//        char testArray[6];
//        uint8 length = 0;
//        if(isTurnComplete()){ //make sure rover is not turning
//            switch(temp){
//                case 0:
//                    length = generateStartForward(testArray, sizeof testArray, UART_COMM, 0x05);
//                    temp++;
//                    break;
//                case 1:
//                    length = generateStartBackward(testArray, sizeof testArray, UART_COMM, 0x06);
//                    temp++;
//                    break;
//                case 2:
//                    length = generateStop(testArray, sizeof testArray, UART_COMM);
//                    temp++;
//                    break;
//                case 3:
//                    //debugNum(2);
//                    length = generateTurnCW(testArray, sizeof testArray, UART_COMM, 0x07);
//                    //FromUARTInt_sendmsg(length, MSGT_UART_DATA, (void*) testArray);
//                    temp++;
//                    break;
//                case 4:
//                    length = generateTurnCCW(testArray, sizeof testArray, UART_COMM, 0x08);
//                    temp++;
//                    break;
//                case 5:
////                    if(start == 0){
////                        length = generateStartFrames(testArray, sizeof testArray, UART_COMM);
////                        start = 1;
////                    }
////                    else{
////                        length = generateStopFrames(testArray, sizeof testArray, UART_COMM);
////                        start = 0;
////                    }
//
//                    temp = 0;
//                    break;
//            }
//
//
//            //uart_send_array(testArray, length);
//            FromUARTInt_sendmsg(length, MSGT_UART_DATA, (void*) testArray);
//        }
        //i2c_master_send(MOTOR_ADDR, length, (char *) frameReq);
        //WriteTimer1(0x4000);
#endif

//#ifdef SENSOR_PIC
////        debugNum(4);
////        pulseUS();
////        transmitData();
////        WriteTimer1(0xFFFF);
//#endif


//#if defined(PICMAN) && defined(DEBUG_ON)
//        char command[6];
//        uint8 length = generateStartForward(command, sizeof command, UART_COMM, 0x10);
//        uart_send_array(command, length);
//#endif

#ifdef MOTOR_PIC

       motor2Ticks++;
       if (ticks2Sent)
       {
           finalMotor2Ticks = 0;
           ticks2Sent = false;
       }
       finalMotor2Ticks++;
       //resetDBG(0);
       if ( motor2Ticks > target2)
       {
           //setDBG(0);
           stop();
           commandDone = true;

           // dont use, slipping errors occur, this is used to adjust speeds of
           // each motor to be the same
           //stopMotor2();      
       }
       WRITETIMER1(0xFFE8);
       
#endif

}

void setM1Tick(uint16_t motor1) {
    target1 = motor1;
    
}
void setM2Tick(uint16_t motor2) {
    target2 = motor2;
}

// set's commandDone to false
void setCommandDone()
{
    commandDone = false;
}

// gets commandDone
bool getCommandDone()
{
    return commandDone;
}

// each interrupt is triggered every 25 ticks so to get the total ticks you want
// the interrupt counter (finalMotor1Ticks) times 25 for the total ticks spun
int getM1Ticks()
{
    ticks1Sent = true;
    return (finalMotor1Ticks * 25);
}


// each interrupt is triggered every 25 ticks so to get the total ticks you want
// the interrupt counter (finalMotor1Ticks) times 25 for the total ticks spun
int getM2Ticks()
{
    ticks2Sent = true;
    return (finalMotor2Ticks * 25);
}

#ifdef SENSOR_PIC
void timer2_int_handler(){
//    debugNum(4);
    addRollover();
}
#endif
