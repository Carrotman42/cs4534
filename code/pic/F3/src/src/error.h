#ifndef ERROR_H
#define	ERROR_H
#include "maindefs.h"
#include "common.h"

#if defined(PICMAN) || defined(MOTOR_PIC) || defined(SENSOR_PIC)
void saveError(uint8 param);
void makeErrorHeaderIfNeeded(uint8 length, char* packedFrameMessage);
#endif

#endif	/* ERROR_H */

