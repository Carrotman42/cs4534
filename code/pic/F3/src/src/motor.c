#include "motor.h"
#include "motorcomm.h"


#include "user_interrupts.h"

#ifdef MOTOR_PIC
uint16_t motor1Ticks = 0;
uint16_t motor2Ticks = 0;

uint16_t target1 = 110;
uint16_t target2 = 107;
bool commandDone = false;
bool killCommand = false;


// both motors in reverse
// if rev = 0 then it goes in reverse infinitely
// if the kill command is true, than we will stop and break out of this function

void reverse(int rev) {
    // initialize variables
    resetTicks();
    setCommandDone();

    // reverse
    calcRevMotor1(rev);
    calcRevMotor2(rev);
    unsigned char test[2] = {0x05, 0x83};
    uart_send_array(&test, 2);

    // wait for reverse to finish or kill command to come in
    while (!getCommandDone() && !killCommand);

    resetKill();
    stop();
                
}

// both motors in forward, slowest speed
// if rev = 0 then it goes in forward infinitely
// if the kill command is true, than we will stop and break out of this function
// speed == 2... 2nd fastest speed
// speed == 3... 3rd fastest speed

void forward(int rev, int speed)
{
    // initialize variables for new command
    resetTicks();
    setCommandDone();

    // move forwards
    calcRevMotor1(rev);
    calcRevMotor2(rev);
    // 2nd fastest speed
    if (speed == 2)
    {
        unsigned char test[2] = {0x62, 0xE0};
        uart_send_array(&test, 2);
    }

    else if (speed == 3)
    {
        // 3rd fastest speed
        unsigned char test[2] = {0x7F, 0xFE};
        uart_send_array(&test, 2);
    }
    // default speed, slowest
    else
    {
        unsigned char test[2] = {0x51, 0xD0};
        uart_send_array(&test, 2);
    }

    // wait for it to be done or the kill flag to be true
    while (!getCommandDone() && !killCommand);

    resetKill();
    stop();

}

void forwardMotor1() {
    // unsigned char test[2] = {0x6A, 0xC0};
    unsigned char test[2] = {0x50, 0xC0};
    uart_send_array(&test, 2);
}

// test function
// motor 1 reverse, motor 2 stops

void reverseMotor1() {
    unsigned char test[2] = {0x21, 0xC0};
    uart_send_array(&test, 2);
}

// test function
// motor 2 forwards, motor 1 stops

void forwardMotor2() {
    //unsigned char test[2] = {0x40, 0xC5};     // REALLY SLOW
    unsigned char test[2] = {0x40, 0xD7};
    uart_send_array(&test, 2);
}

// test function
// motor 2 reverse, motor 1 stops

void reverseMotor2() {
    unsigned char test[2] = {0x40, 0xB0};
    uart_send_array(&test, 2);
}

// resets kill flag to false and stops
void killAndStop()
{
    resetKill();
    stop();
}

// stops both motors

void stop() {
    // 0x00 also stops both wheels
    unsigned char test[2] = {0x40, 0xC0};
    uart_send_array(&test, 2);
}

// test function
// stops motor 1

void stopMotor1() {
    unsigned char test[1] = {0x40};
    uart_send_array(&test, 1);
}

// test function
// stops motor 2

void stopMotor2() {
    unsigned char test[1] = {0xC0};
    uart_send_array(&test, 1);
}

// calculate the target (motor 1) which is the desired number of ticks for x revolutions
// x = number of revolutions
// look up table for x revolutions in motor 1
// 0 = going straight or reverse for a very long time

void calcRevMotor1(int x) {
    //target1 = (110 * x) + (15 * x) - 15;
    if (x == 0)
        target1 = 0xFFFF; // 10E10
    else if (x == 1)
        target1 = 116;
    else if (x == 2)
        target1 = 238;
    else if (x == 3)
        target1 = 360;
    else if (x == 4)
        target1 = 482;
    else if (x == 5)
        target1 = 604;
    else if (x == 6)
        target1 = 726;
    else if (x == 7)
        target1 = 848;
    else if (x == 8)
        target1 = 970;
    else if (x == 9)
        target1 = 1092;
    else
        target1 = (116 * x) + (6 * x) - 6;
    setM1Tick(target1);
}

// calculate the target (motor 2) which is the desired number of ticks for x revolutions
// x = number of revolutions
// look up table for x revolutions in motor 2
// 0 = going straight or reverse for a very long time

void calcRevMotor2(int x) {
    //target2 = (107 * x) + (9 * x) - 8;
    if (x == 0)
        target2 = 0xFFFF; // 10E10
    else if (x == 1)
        target2 = 115;
    else if (x == 2)
        target2 = 235;
    else if (x == 3)
        target2 = 355;
    else if (x == 4)
        target2 = 475;
    else if (x == 5)
        target2 = 600;
    else if (x == 6)
        target2 = 720;
    else if (x == 7)
        target2 = 843;
    else if (x == 8)
        target2 = 963;
    else if (x == 9)
        target2 = 1083;
    else
        target2 = (115 * x) + (5 * x) - 5 + 8;
    setM2Tick(target2);
}

// turns right 90 degrees on the spot
// than moves forwards for 2 revolutions to grab sensor data from the wall/bricks
// stops once its turned and moved forwards

void turnRight()
{
    // initialize variables for the new command to start
    setCommandDone();
    resetTicks();

    // turn right
    target1 = 110;
    target2 = 107;
    unsigned char test[2] = {0x62, 0xA1};
    uart_send_array(test, 2);

    while (!getCommandDone());      // wait for the turn to complete
    setCommandDone();               // turn flag to false for next command
    resetTicks();                   // reset the ticks for the next command


    // move forwards 2 revolutions
     calcRevMotor1(2);
     calcRevMotor2(2);
     unsigned char test[2] = {0x51, 0xD0};
     uart_send_array(test, 2);

     // wait for the 2 revolutions to be done or the kill flag to be true
     while (!getCommandDone() );
     resetKill();
  
     stop();            // done with turning

}

// turns left 90 degrees on the spot
// moves forwards 2 revolutios to pick up sensor data from the walls/bricks
// stops after both steps are taken
void turnLeft()
{
    // initialize variables for the new command to start
    setCommandDone();
    resetTicks();

    // Turn 90 Degrees to the left
    target1 = 110;
    target2 = 107;
    unsigned char test[2] = {0x18, 0xE0};
    uart_send_array(test, 2);

    while (!getCommandDone());      // wait for the turn to complete
    setCommandDone();               // turn flag to false for next command
    resetTicks();                   // reset the ticks for the next command


    // readjust so the turn is as close as possible
    target1 = 2;
    target2 = 2;
    unsigned char test[2] = {0x62, 0xA0};
    uart_send_array(test, 2);


    while (!getCommandDone() );      // wait for readjustment to be complete
    setCommandDone();
    resetTicks();


    // move forwards 2 revolutions
     calcRevMotor1(2);
     calcRevMotor2(2);
     unsigned char test[2] = {0x51, 0xD0};
     uart_send_array(test, 2);

     // wait for the 2 revolutions to be done or the kill flag to be true
     while (!getCommandDone() );     
     resetKill();

     stop();            // done with turning
}

// readjusts the rover to the left by a little bit, approx. 1-2 degrees
// no kill possible here

void readjustLeft() {
    // initialize variables
    resetKill();
    resetTicks();
    setCommandDone();

    target1 = 2;
    target2 = 2;
    unsigned char test[2] = {0x18, 0xE0};
    uart_send_array(&test, 2);

    while (!getCommandDone());      // wait for the readjustment to be done

    stop();
}

// readjusts the rover to the right by a little bit, approx. 1-2 degrees

void readjustRight() {

    // initialize variables
    resetKill();
    resetTicks();
    setCommandDone();

    target1 = 2;
    target2 = 2;
    unsigned char test[2] = {0x62, 0xA0};
    uart_send_array(&test, 2);

    while (!getCommandDone());      // wait for the readjustment to be done

    stop();

}

// test function (NOT USED)
// moves the rover forwards by half a revolution

void forwardHalfRev() {
    resetKill();
    target1 = 49;
    target2 = 55;
    unsigned char test[2] = {0x52, 0xD0};
    uart_send_array(&test, 2);
}

// spins in place for victory dance for a long time if 0 is the parameter
// stops once the kill command is true or the given revolutions is done

void funFunc(int rev) {
    resetTicks();
    setCommandDone();

    // dance function
    calcRevMotor1(rev);
    calcRevMotor2(rev);
    unsigned char test[2] = {0x7F, 0x80};
    uart_send_array(&test, 2);

    // wait for the kill command or the command to be done
    while (!getCommandDone() && !killCommand);

    resetKill();
    stop();
}

// resets the ticks to 0

void resetTicks() {
    motor1Ticks = 0;
    motor2Ticks = 0;
}

// gets the ticks from user_interrupts and stores them here for the ARM
// to grab whenever

int getMotor1Ticks() {
    return getM1Ticks();
}

// gets the ticks from user_interrupts and stores them here for the ARM
// to grab whenever

int getMotor2Ticks() {
    return getM2Ticks();
}

// sets the kill command to true, stops most functions

void setKill() {
    killCommand = true;
}

// resets the kill command to false, to start most functions

void resetKill() {
    killCommand = false;
}

#endif