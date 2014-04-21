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
#ifdef SENSOR_PIC

    void initializeColorSensor(void);
    void clearColorSensorInterrupt(void);

#endif
#ifdef	__cplusplus
}
#endif

#endif	/* COLOR_SENSOR_H */

