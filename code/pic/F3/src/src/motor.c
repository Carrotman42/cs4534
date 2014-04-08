#include "motor.h"
#include "motorcomm.h"

uint16_t motor1Ticks = 0;
uint16_t motor2Ticks = 0;

uint16_t target1 = 110;
uint16_t target2 = 107;

int finalMotor1Ticks = -1;      // -1 if there was an error
int finalMotor2Ticks = -1;      // -1 if there was an error
// both motors in reverse
void reverse(){
    resetTicks();
    STATES state = REVERSE;
    while (state != IDLE){
        switch (state) {
            case REVERSE:
                if (!commandDone){
                    unsigned char test[2] = {0x05, 0x83};
                    uart_send_array(&test, 2);
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;
            case FINISHED:
                stop();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }

//    unsigned char test[2] = {0x05, 0x83};
//    uart_send_array(&test, 2);
}

// both motors in forward
void forward(){
    resetTicks();
    STATES state = FORWARDS;
    while (state != IDLE){
        switch (state) {
            case FORWARDS:
                if (!commandDone){
                    unsigned char test[2] = {0x52, 0xD0};
                    uart_send_array(&test, 2);
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;
            case FINISHED:
                stop();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }



    //unsigned char test[2] = {0x70, 0xFF};
//    unsigned char test[2] = {0x52, 0xD0};
//    uart_send_array(&test, 2);
}

// both motors in forward,  2nd fastest forward
void forward2(){
    resetTicks();
    STATES state = FORWARDS;
    while (state != IDLE){
        switch (state){
            case FORWARDS:
                if (!commandDone){
                    unsigned char test[2] = {0x62, 0xE0};
                    uart_send_array(&test, 2);
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;
            case FINISHED:
                stop();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }

//    unsigned char test[2] = {0x62, 0xE0};
//    uart_send_array(&test, 2);
}

// both motors in forward, fastest forward
void forward3(){
    resetTicks();
    STATES state = FORWARDS;
    while (state != IDLE){
        switch (state){
            case FORWARDS:
                if (!commandDone){
                    unsigned char test[2] = {0x7F, 0xFE};
                    uart_send_array(&test, 2);
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;
            case FINISHED:
                stop();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }


//    unsigned char test[2] = {0x7F, 0xFF};
//    uart_send_array(&test, 2);
}

// motor 1 forwards, motor 2 stops
void forwardMotor1(){
    // unsigned char test[2] = {0x6A, 0xC0};
    unsigned char test[2] = {0x50 , 0xC0};
    uart_send_array(&test, 2);
}

// motor 1 reverse, motor 2 stops
void reverseMotor1(){
    unsigned char test[2] = {0x21, 0xC0};
    uart_send_array(&test, 2);
}

// motor 2 forwards, motor 1 stops
void forwardMotor2(){
    //unsigned char test[2] = {0x40, 0xC5};     // REALLY SLOW
    unsigned char test[2] = {0x40, 0xD7};
    uart_send_array(&test, 2);
}

// motor 2 reverse, motor 1 stops
void reverseMotor2(){
    unsigned char test[2] = {0x40, 0xB0};
    uart_send_array(&test, 2);
}

// stops both motors
void stop(){
    // 0x00 stops both wheels
    unsigned char test[2] = {0x40, 0xC0};
    uart_send_array(&test, 2);
}

// stops motor 1
void stopMotor1(){
    unsigned char test[1] = {0x40};
    uart_send_array(&test,1);
}

// stops motor 2
void stopMotor2(){
    unsigned char test[1] = {0xC0};
    uart_send_array(&test,1);
}

// calculate the target (motor 1) which is the desired number of ticks for x revolutions
// x = number of revolutions
void calcRevMotor1(int x){
    target1 = (110*x) + (15*x) - 15;
}

// calculate the target (motor 2) which is the desired number of ticks for x revolutions
// x = number of revolutions
void calcRevMotor2(int x)
{
    target2 = (107*x) + (9*x) - 8;
}

// turns right 90 degrees on the spot
void turnRight90_onSpot(){
    // TODO: move forwards half a revolution before turning
    stop();
    resetTicks();
    STATES state = TURN;
    while (state != IDLE) {
        switch (state) {
            case TURN:
                if (!commandDone) {
                    // Turn 90 Degrees to the right
                    calcRevMotor1(1);
                    calcRevMotor2(1);
                    unsigned char test[2] = {0x62, 0xA1};
                    uart_send_array(&test, 2);
                } else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = READJUSTMENT;
                }
                break;
            case READJUSTMENT:
                // readjustment code
                if (!commandDone) {
                    // readjust 1 angle for 90 degrees perfectly
                    target1 = 5;
                    target2 = 5;
                    unsigned char test[2] = {0x62, 0xA1};
                    uart_send_array(&test, 2);
                } else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = MOVE_FORWARDS;
                }
                break;
            case MOVE_FORWARDS:
                if (!commandDone){
                    calcRevMotor1(2);
                    calcRevMotor2(2);
                    forward();
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;            
            case FINISHED:
                stop();
                turnCompleted();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }
//    calcRevMotor1(1);
//    calcRevMotor2(1);
//    unsigned char test[2] = {0x62, 0xA1};
//    uart_send_array(&test, 2);

    // TODO: move motor 1 a few ticks to adjust for the half degree error
}

// turns left 90 degrees on the spot
void turnLeft90_onSpot()
{
    // TODO: move forwards half a revolution before turning
    resetTicks();
    STATES state = TURN;
    while (state != IDLE) {
        switch (state) {
            case TURN:
                if (!commandDone) {
                    // Turn 90 Degrees to the right
                    //target1 = 90;
                    calcRevMotor1(1);
                    calcRevMotor2(1);
                    unsigned char test[2] = {0x18, 0xE0};
                    uart_send_array(&test, 2);
                } else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = READJUSTMENT;
                }
                break;
           case READJUSTMENT:
                // Don't need readjustment anymore.... works fine//
//                // TODO: insert readjustment code
                if (!commandDone) {
                    // readjust 1 angle for 90 degrees perfectly
                    target1 = 5;
                    target2 = 5;
                    unsigned char test[2] = {0x62, 0xA0};
                    uart_send_array(&test, 2);
                } else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = MOVE_FORWARDS;
                }
                break;
            case MOVE_FORWARDS:
                if (!commandDone){
                    calcRevMotor1(2);
                    calcRevMotor2(2);
                    forward();
                }
                else {
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }
                break;
            case FINISHED:
                stop();
                turnCompleted();
                state = IDLE;
                break;
            case IDLE:
                break;
        }
    }
}

// readjusts the motor to the left by a little bit
void readjustLeft(){
    STATES state = READJUSTMENT;
    while(1){
        switch (state){
            case READJUSTMENT:
                if (!commandDone){
                    target1 = 5;
                    target2 = 5;
                    unsigned char test[2] = {0x18, 0xE0};
                    uart_send_array(&test, 2);
                }
                else{
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }

                break;
            case FINISHED:
                stop();
                break;
        }
    }
}

// readjusts the motor to the right by a little bit
void readjustRight(){
    STATES state = READJUSTMENT;
    while(1){
        switch (state){
            case READJUSTMENT:
                if (!commandDone){
                    target1 = 5;
                    target2 = 5;
                    unsigned char test[2] = {0x62, 0xA0};
                    uart_send_array(&test, 2);
                }
                else{
                    stop();
                    resetTicks();
                    commandDone = false;
                    state = FINISHED;
                }

                break;
            case FINISHED:
                stop();
                break;
        }
    }
}

void forwardHalfRev(){
    target1 = 49;
    target2 = 55;
    unsigned char test[2] = {0x52, 0xD0};
    uart_send_array(&test, 2);
}

// spins in place for victory dance
void funFunc(){
    unsigned char test[2] = {0x7F, 0x80};
    uart_send_array(&test, 2);
}

// resets the ticks to 0
void resetTicks(){
    motor1Ticks = 0;
    motor2Ticks = 0;
}

int getMotor1Ticks(){
    return finalMotor1Ticks;
}

int getMotor2Ticks(){
    return finalMotor2Ticks;
}