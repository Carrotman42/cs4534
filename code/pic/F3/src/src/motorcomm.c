#include "motorcomm.h"
#include "motor.h"

//returns 1 if success, 0 if error
#ifndef SENSOR_PIC //unnecessary for sensor pic
uint8 sendMotorAckResponse(uint8 parameters, uint8 msgid, uint8 wifly){
    char outbuf[10] = {0};

    uint8 success = 1;
    uint8 bytes_packed = 0;
    switch(parameters){ 
        case 0x00:
            bytes_packed = packStartForwardAck(outbuf, sizeof outbuf, msgid);
            break;
        case 0x01:
            bytes_packed = packStartBackwardAck(outbuf, sizeof outbuf, msgid);
            break;
        case 0x02:
            bytes_packed = packStopAck(outbuf, sizeof outbuf, msgid);
            break;
        case 0x03:
            bytes_packed = packTurnCWAck(outbuf, sizeof outbuf, msgid);
//            if(!wifly)
//                makeHighPriority(outbuf);
            break;
        case 0x04:
            bytes_packed = packTurnCCWAck(outbuf, sizeof outbuf, msgid);
//            if(!wifly)
//                makeHighPriority(outbuf);
            break;
        case 0x06:
            bytes_packed = packReadjustCWAck(outbuf, sizeof outbuf, msgid);
            break;
        case 0x07:
            bytes_packed = packReadjustCCWAck(outbuf, sizeof outbuf, msgid);
            break;
        case 0x08:
            bytes_packed = packGoForwardDistanceTurnAck(outbuf, sizeof outbuf, msgid);
            break;
        default:
            bytes_packed = packPICDetectErrorAck(outbuf, sizeof outbuf, msgid);
            success = 0;
            break;
    }
    sendData(outbuf, bytes_packed, wifly);
    return success;
}
#endif

#ifdef MOTOR_PIC
void sendEncoderData(uint8 msgid){
//    addEncoderData(0x01,0x2,0x03,0x04);
//    addEncoderData(0x01,0x2,0x03,0x04);//make sure it adds properly
    int motorTicks1 = getMotor1Ticks(); //left
    int motorTicks2 = getMotor2Ticks(); //right
    addEncoderData(highByte(motorTicks1), lowByte(motorTicks1), highByte(motorTicks2), lowByte(motorTicks2));
    sendFrameData(msgid);
    setResetEncoderData(); //we want to reset the encoder data in main now that 
}
#endif

#if defined(MASTER_PIC) || defined(ARM_EMU) || defined(PICMAN) || defined(MOTOR_PIC)
static uint8 turnDone = 1;
void turnStarted(){
    turnDone = 0;
}
void turnCompleted(){
    turnDone = 1;
}
uint8 isTurnComplete(){
    return turnDone;
}
#endif