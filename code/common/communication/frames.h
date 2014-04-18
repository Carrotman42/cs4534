#ifndef FRAMES_H
#define	FRAMES_H
#include "maindefs.h"
#include "sensor_types.h"
#include "brain_rover.h"

#define u2_8to16(v) makeInt(v[0], v[1])
#define makeInt(high, low) (((int) high) << 8 | (low))
#define highByte(c) (c >> 8)
#define lowByte(c) (c & 0xFF)

#ifdef MOTOR_PIC
void setResetEncoderData();
#endif


//every pic has its own definition of a frame
#if defined(PICMAN) || defined(MASTER_PIC) || defined(ARM_EMU) || defined(ROVER_EMU) || defined(ARM)
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
void sendFrameData();
#endif

#ifdef MASTER_PIC
void waitForSensorFrame();
#endif

#if defined(PICMAN) || defined(MOTOR_PIC) || defined(SENSOR_PIC)
void sendFrameData(uint8 msgid);
#endif

#ifndef ARM_EMU
uint8 frameDataReady();
void clearFrameData();
#endif

#if defined(ROVER_EMU) && defined(DEBUG_ON)
void fillDummyFrame();
#endif

#endif	/* FRAMES_H */

