#include "motorcomm.h"

void sendMotorAckResponse(uint8 parameters, uint8 wifly){
    char outbuf[5];
    switch(parameters){
        case 0x00:
            packStartForwardAck(outbuf, 5, wifly);
            break;
        case 0x01:
            packStartBackwardAck(outbuf, 5, wifly);
            break;
        case 0x02:
            packStopAck(outbuf, 5, wifly);
            break;
        case 0x03:
            packTurnCWAck(outbuf, 5, wifly);
            break;
        case 0x04:
            packTurnCCWAck(outbuf, 5, wifly);
            break;
        default:
            packPICDetectErrorAck(outbuf, 5, wifly);
            break;
    }
    sendData(outbuf, 5, wifly);
}
