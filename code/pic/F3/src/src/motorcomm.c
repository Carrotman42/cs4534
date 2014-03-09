#include "motorcomm.h"

//returns 1 if success, 0 if error
#ifndef SENSOR_PIC //unnecessary for sensor pic
uint8 sendMotorAckResponse(uint8 parameters, uint8 msgid, uint8 wifly){
    char outbuf[10];

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
            break;
        case 0x04:
            bytes_packed = packTurnCCWAck(outbuf, sizeof outbuf, msgid);
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
void sendEncoderData(){
    char data[4]; //will need a function to actually get the encoder data.
                         //For now, send dummy values
//    data[0].data = 0x01;
//    data[1].data = 0x02;
//    data[2].data = 0x03;
//    data[3].data = 0x04;

    addEncoderData(0x01,0x02,0x03,0x04);
    packFrame(data, sizeof data);


    char outbuf[MAX_I2C_SENSOR_DATA_LEN + HEADER_MEMBERS];
    uint8 bytes_packed = packEncoderData(data, sizeof data, outbuf, sizeof outbuf);
    sendData(outbuf, bytes_packed, I2C_COMM);
}
#endif