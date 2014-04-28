/* 
 * File:   color_sensor.h
 * Author: mcantrell
 *
 * Created on April 21, 2014, 3:54 AM
 */

#ifndef COLOR_SENSOR_H
#define	COLOR_SENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "maindefs.h"
#ifdef MASTER_PIC
#define ENABLE 0
#define INT_ENABLE 1
#define THRESHOLD_LOW 2
#define THRESHOLD_HIGH 3
#define PERSISTENCE 4
#define INT_CLEAR 5
    
    extern uint8 colorSensorInitStage;

    void initializeColorSensor(void);
//    void initializeColorSensor(uint16 upperThresh, uint16 lowerThresh, char persistence);
    void clearColorSensorInterrupt(void);
    void initializeColorSensorStage(void);

#endif
#ifdef	__cplusplus
}
#endif

#endif	/* COLOR_SENSOR_H */

