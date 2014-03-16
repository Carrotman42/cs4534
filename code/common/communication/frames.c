#include "frames.h"

#ifdef MASTER_PIC
static uint8 framesRequested = 0;
static uint8 sensorDataSet;
static uint8 encoderDataSet;
#endif
static Frame frame;


#if defined(SENSOR_PIC) || defined(MASTER_PIC)
void addSensorFrame(uint8 ultrasonic, uint8 IR1, uint8 IR2){
    frame.ultrasonic = ultrasonic;
    frame.IR1 = IR1;
    frame.IR2 = IR2;
#ifdef MASTER_PIC
    sensorDataSet = 1;
#endif
}
#endif

#if defined(MOTOR_PIC) || defined(MASTER_PIC)
void addEncoderData(uint8 encoderRightHB, uint8 encoderRightLB, uint8 encoderLeftHB, uint8 encoderLeftLB){
    frame.encoderRight[0] = encoderRightHB;
    frame.encoderRight[1] = encoderRightLB;
    frame.encoderLeft[0] = encoderLeftHB;
    frame.encoderLeft[1] = encoderLeftLB;
#ifdef MASTER_PIC
    encoderDataSet = 1;
#endif
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

//returns 1 if both datas have been set, frame is full AND start frames has been sent
//0 if either hasnt been set yet
uint8 frameDataReady(){
    return framesRequested && sensorDataSet && encoderDataSet;
}

//sends the data over uart
void sendFrameData(){
    debugNum(2);
    char packedFrame[FRAME_MEMBERS] = "";
    uint8 bytes_packed = packFrame(packedFrame, sizeof packedFrame); //puts frame into char array
    if(bytes_packed == 0) return;
    char packedFrameMessage[FRAME_MEMBERS + HEADER_MEMBERS] = "";
    int length = packFrameData(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage); //adds the headers to the data
    uart_send_array(packedFrameMessage, length);
}

//the frames won't be sent or used unless these values are 1
//effectively, the frames are reset since they'll  be rewritten before used next.
void clearFrameData(){
    sensorDataSet = 0;
    encoderDataSet = 0;
}
#endif

#ifdef PICMAN
void sendFrameData(){
    char packedFrame[FRAME_MEMBERS];
    uint8 bytes_packed = packFrame(packedFrame, sizeof packedFrame); //puts frame into char array
    if(bytes_packed == 0) return;
    char packedFrameMessage[FRAME_MEMBERS + HEADER_MEMBERS];
    int length = packReadFrame(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage); //adds the headers to the data
    //only way this will get called is if it's an i2c response (from arm)

    start_i2c_slave_reply(length, packedFrameMessage);
}
#ifdef DEBUG_ON
void fillDummyFrame(){
    frame.ultrasonic = 0x05;
    frame.IR1 = 0x06;
    frame.IR2 = 0x07;
    frame.encoderRight[0] = 0x01;
    frame.encoderRight[1] = 0x02;
    frame.encoderLeft[0] = 0x03;
    frame.encoderLeft[1] = 0x04;
}
#endif
#endif
