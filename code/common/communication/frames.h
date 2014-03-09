#include "../../pic/F3/src/src/maindefs.h"

#ifndef FRAMES_H
#define	FRAMES_H
#include "../sensor_types.h"
#include "brain_rover.h"
#include "../../pic/F3/src/src/my_uart.h"
#include "../../pic/F3/src/src/debug.h"

//every pic has its own definition of a frame
#if defined(PICMAN) || defined(MASTER_PIC)
typedef struct {
    uint8 ultrasonic;
    uint8 IR1;
    uint8 IR2;
    uint8 encoderRight[2];
    uint8 encoderLeft[2];
} Frame;
#define FRAME_MEMBERS 7
#endif

#if defined(SENSOR_PIC)
typedef struct {
    uint8 ultrasonic;
    uint8 IR1;
    uint8 IR2;
} Frame;
#define FRAME_MEMBERS 3
#endif

#if defined(MOTOR_PIC)
typedef struct {
    uint8 encoderLeft[2];
    uint8 encoderRight[2];
} Frame;
#define FRAME_MEMBERS 4
#endif

#if defined(SENSOR_PIC) || defined(MASTER_PIC)
void addSensorFrame(uint8 ultrasonic, uint8 IR1, uint8 IR2);
#endif
#if defined(MOTOR_PIC) || defined(MASTER_PIC)
void addEncoderData(uint8 encoderRightHB, uint8 encoderRightLB, uint8 encoderLeftHB, uint8 encoderLeftLB);
#endif
uint8 packFrame(char* out, uint8 maxout);
#ifdef MASTER_PIC
void startFrames();
void stopFrames();
uint8 frameDataReady();
void sendFrameData();
void clearFrameData();
#endif

#endif	/* FRAMES_H */

