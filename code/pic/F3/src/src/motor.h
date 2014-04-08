/* 
 * File:   motor.h
 * Author: Glen
 *
 * Created on March 2, 2014, 9:05 PM
 */

#include "my_uart.h"
#include <stdint.h>
#include <stdbool.h>
#ifndef MOTOR_H
#define	MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum  {FORWARDS, REVERSE, MOVE_FORWARDS, TURN, READJUSTMENT, FINISHED }STATES;
    bool commandDone = false;

     extern uint16_t motor1Ticks;
     extern uint16_t motor2Ticks;
    extern int finalMotor1Ticks;      // -1 if there was an error
    extern int finalMotor2Ticks;      // -1 if there was an error
    // motor 1 ticks for 1 revolution: 2750
    // motor 2 ticks for 1 revolution: 2675
    // target values: 1 interrupt = 25 ticks
     extern uint16_t target1;    // 110 for 1 revolution
     extern uint16_t target2;    // 107 for 1 revolution but an offset of 1 in formula

    void reverse();
    void forward();
    void forward2();
    void forward3();
    void forwardMotor1();
    void reverseMotor1();
    void forwardMotor2();
    void reverseMotor2();
    void stop();
    void stopMotor1();
    void stopMotor2();
    void calcRevMotor1(int x);
    void calcRevMotor2(int x);
    void turnRight90_onSpot();
    void turnLeft90_onSpot();
    void forwardHalfRev();
    void funFunc();
    void readjustLeft();
    void readjustRight();

    void resetTicks();
    int getMotor1Ticks();
    int getMotor2Ticks();


#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

