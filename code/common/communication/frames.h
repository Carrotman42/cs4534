#include "maindefs.h"

#ifndef FRAMES_H
#define	FRAMES_H
#include "sensor_types.h"
#include "brain_rover.h"

//every pic has its own definition of a frame
#if defined(PICMAN) || defined(MASTER_PIC) || defined(ARM_EMU) || defined(ROVER_EMU)
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
    uint8 encoderRight[2];
    uint8 encoderLeft[2];
} Frame;
#define FRAME_MEMBERS 4
#endif

#if defined(SENSOR_PIC) || defined(MASTER_PIC) || defined(PICMAN) || defined(ROVER_EMU)
void addSensorFrame(uint8 ultrasonic, uint8 IR1, uint8 IR2);
#endif
#if defined(MOTOR_PIC) || defined(MASTER_PIC) || defined(PICMAN) || defined(ROVER_EMU)
void addEncoderData(uint8 encoderRightHB, uint8 encoderRightLB, uint8 encoderLeftHB, uint8 encoderLeftLB);
#endif
uint8 packFrame(char* out, uint8 maxout);
#if defined(MASTER_PIC) || defined(ROVER_EMU)
void startFrames();
void stopFrames();
uint8 frameDataReady();
void sendFrameData();
void clearFrameData();
#endif

#ifdef PICMAN
void sendFrameData();
uint8 frameDataReady();
void clearFrameData();
#endif

#if defined(ROVER_EMU) && defined(DEBUG_ON)
void fillDummyFrame();
#endif

#endif	/* FRAMES_H */

