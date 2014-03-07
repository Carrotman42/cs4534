#include "frames.h"

#ifdef MASTER_PIC
static uint8 framesRequested = 0;
#endif
static Frame frame;


#if defined(SENSOR_PIC) || defined(MASTER_PIC)
void addSensorFrame(uint8 ultrasonic, uint8 IR1, uint8 IR2){
    frame.ultrasonic = ultrasonic;
    frame.IR1 = IR1;
    frame.IR2 = IR2;
}
#endif

#if defined(MOTOR_PIC) || defined(MASTER_PIC)
void addEncoderData(uint8 encoderLeftHB, uint8 encoderLeftLB, uint8 encoderRightHB, uint8 encoderRightLB){
    frame.encoderLeft[0] = encoderLeftHB;
    frame.encoderLeft[1] = encoderLeftLB;
    frame.encoderRight[0] = encoderRightHB;
    frame.encoderRight[1] = encoderRightLB;
}
#endif

//returns number of bytes loaded into the buffer
uint8 packFrame(char* out, uint8 maxout){
    if(maxout < FRAME_MEMBERS) return 0;
    uint8 i = 0;
    char* framePointer = (char*)&frame;
    for(i; i < FRAME_MEMBERS; i++){
        out[i] = *framePointer++;
    }
    return FRAME_MEMBERS;
}

#ifdef MASTER_PIC
void startFrames(){
    framesRequested = 1;
}

void stopFrames(){
    framesRequested = 0;
}
#endif