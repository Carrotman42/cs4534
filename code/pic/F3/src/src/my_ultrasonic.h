/* 
 * File:   my_ultrasonic.h
 * Author: Michael
 *
 * Created on April 4, 2014, 3:02 AM
 */

#ifndef MY_ULTRASONIC_H
#define	MY_ULTRASONIC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initUS();
    void pulseUS();
    void startTimerUS();
    void stopTimerUS();
    char* getDistanceUS();

    void us_int_handler();


#ifdef	__cplusplus
}
#endif

#endif	/* MY_ULTRASONIC_H */

