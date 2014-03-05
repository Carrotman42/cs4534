
#ifndef SENSOR_TYPES_H_INC
#define SENSOR_TYPES_H_INC


// If these types (char/short) are different sizes on pic/arm, we'll have to extract these
//    to a common platform.h stuff
typedef char uint8;
typedef short uint16;


typedef struct {
    uint8 data;
} sensorADData;

typedef struct{
    uint8 data;
} encoderData;

#endif