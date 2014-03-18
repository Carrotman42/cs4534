/* 
 * File:   motor.h
 * Author: Glen
 *
 * Created on March 2, 2014, 9:05 PM
 */

#include "my_uart.h"
#include <stdint.h>
#ifndef MOTOR_H
#define	MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

    volatile extern uint16_t motor1Ticks = 0;
    volatile extern uint16_t motor2Ticks = 0;

    void reverse();
    void forward();
    void forwardMotor1();
    void reverseMotor1();
    void forwardMotor2();
    void reverseMotor2();
    void stop();

    void motor0_int_handler();
    void motor1_int_handler();


#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

