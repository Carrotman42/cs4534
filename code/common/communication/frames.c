#include "frames.h"
#include "error.h"


#ifndef MOTOR_PIC
static uint8 sensorDataSet;
#endif
#ifndef SENSOR_PIC
static uint8 encoderDataSet;
#endif
#ifdef MOTOR_PIC
static uint8 resetEncoderData;
void setResetEncoderData(){
    resetEncoderData = 1;
}
#endif


#if defined(MASTER_PIC) || defined(ROVER_EMU) || defined(PICMAN)
static uint8 framesRequested = 0;
#endif
static Frame frame;


#if defined(SENSOR_PIC) || defined(MASTER_PIC) || defined(PICMAN) || defined(ROVER_EMU)
void addSensorFrame(uint8 ultrasonic, uint8 IR1, uint8 IR2){
    frame.ultrasonic = ultrasonic;
    frame.IR1 = IR1;
    frame.IR2 = IR2;
    sensorDataSet = 1;
}
#endif

#if defined(MOTOR_PIC) || defined(MASTER_PIC) || defined(PICMAN) || defined(ROVER_EMU)
void addEncoderData(uint8 encoderRightHB, uint8 encoderRightLB, uint8 encoderLeftHB, uint8 encoderLeftLB){
#ifdef MOTOR_PIC
    if(resetEncoderData){
        clearFrameData(); //do not accummulate
        resetEncoderData = 0;
    }
#endif
    int rightNew = makeInt(encoderRightHB, encoderRightLB);
    int leftNew = makeInt(encoderLeftHB, encoderLeftLB);
    int rightOld = makeInt(frame.encoderRight[0], frame.encoderRight[1]);
    int leftOld = makeInt(frame.encoderLeft[0], frame.encoderLeft[1]);

    rightNew += rightOld;
    leftNew += leftOld;
    
    frame.encoderRight[0] = highByte(rightNew);
    frame.encoderRight[1] = lowByte(rightNew);
    frame.encoderLeft[0] = highByte(leftNew);
    frame.encoderLeft[1] = lowByte(leftNew);
    encoderDataSet = 1;
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

#if defined(MASTER_PIC) || defined(ROVER_EMU)
void startFrames(){
    framesRequested = 1;
}

void stopFrames(){
    framesRequested = 0;
}

//sends the data over uart
void sendFrameData(){
    char packedFrame[FRAME_MEMBERS] = {0};
    uint8 bytes_packed = packFrame(packedFrame, sizeof packedFrame); //puts frame into char array
    if(bytes_packed == 0) return;
    char packedFrameMessage[FRAME_MEMBERS + HEADER_MEMBERS] = {0};
    int length = packFrameData(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage); //adds the headers to the data
    uart_send_array(packedFrameMessage, length);
    clearFrameData(); //prepare for next sending
}

#endif


#if defined(PICMAN) || defined(MOTOR_PIC) || defined(SENSOR_PIC)

void sendFrameData(uint8 msgid){
    //this block may be unnecessary but it makes all invalid data easier to see for now
    if(!frameDataReady()){
        clearFrameData(); //need to change this - this will reset all data already gathered
    }
    char packedFrame[FRAME_MEMBERS] = {0};
    uint8 bytes_packed = packFrame(packedFrame, sizeof packedFrame); //puts frame into char array
    if(bytes_packed == 0) return;
    char packedFrameMessage[FRAME_MEMBERS + HEADER_MEMBERS] = {0};
#ifdef PICMAN
    int length = packReadFrame(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage, msgid); //adds the headers to the data
#elif defined(MOTOR_PIC)
    int length = packEncoderData(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage, msgid);
#elif defined(SENSOR_PIC)
    int length = packSensorFrame(packedFrame, sizeof packedFrame, packedFrameMessage, sizeof packedFrameMessage, msgid);
#endif
    if(length != 0){
        makeErrorHeaderIfNeeded(length, packedFrameMessage);
        //must set flags after adding error header(if needed)
        if(!frameDataReady()){
            flagInvalidData(packedFrameMessage);
        }
#ifdef PICMAN
        if(isColorSensorTriggered()){
            flagColorSensed(packedFrameMessage);
            clearColorSensorStatus(); //don't need to send multiple times
        }
#endif
        //only way this will get called is if it's an i2c response (from arm or master pic)
        start_i2c_slave_reply(length, packedFrameMessage);
#ifndef MOTOR_PIC //the motor pic won't reset when sending because it'll send in the interrupt handler
        clearFrameData(); //data sent - prepare for next sending
#endif
    }
}
#endif

#ifndef ARM_EMU
//returns 1 if both datas have been set, frame is full AND start frames has been sent
//0 if either hasnt been set yet
uint8 frameDataReady(){
#if defined(MASTER_PIC) ||defined(ROVER_EMU)
    return /*framesRequested && */sensorDataSet && encoderDataSet;
#elif defined(PICMAN)
    return sensorDataSet && encoderDataSet;
#elif defined(MOTOR_PIC)
    return encoderDataSet;
#elif defined(SENSOR_PIC)
    return sensorDataSet;
#else
    return 0;
#endif
}

//the frames won't be sent or used unless these values are 1
//effectively, the frames are reset since they'll  be rewritten before used next.
void clearFrameData(){
#ifdef MASTER_PIC //The master pic does not want to set the sensor frame to 0.  
    sensorDataSet = 0;
#endif
#if defined(PICMAN) || defined(SENSOR_PIC) ||defined(ROVER_EMU)
    addSensorFrame(0,0,0);
    sensorDataSet = 0;
#endif
#if defined(PICMAN) || defined(MOTOR_PIC) || defined(MASTER_PIC) ||defined(ROVER_EMU)
    frame.encoderLeft[0] = 0;
    frame.encoderLeft[1] = 0;
    frame.encoderRight[0] = 0;
    frame.encoderRight[1] = 0;
    encoderDataSet = 0;
#endif
}
#endif


#if defined(ROVER_EMU) && defined(DEBUG_ON)
void fillDummyFrame(){
    addSensorFrame(0x05, 0x06, 0x07);
    addEncoderData(0x01, 0x02, 0x03, 0x04);
}
#endif
