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

    void initializeColorSensor(void);
//    void initializeColorSensor(uint16 upperThresh, uint16 lowerThresh, char persistence);
    void clearColorSensorInterrupt(void);

#endif
#ifdef	__cplusplus
}
#endif

#endif	/* COLOR_SENSOR_H */

