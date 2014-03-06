/* 
 * File:   motor.h
 * Author: Glen
 *
 * Created on March 2, 2014, 9:05 PM
 */

#include "my_uart.h"
#ifndef MOTOR_H
#define	MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

    void reverse();
    void forward();
    void forwardMotor1();
    void reverseMotor1();
    void forwardMotor2();
    void reverseMotor2();
    void stop();


#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

