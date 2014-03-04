#include "comm.h"

#define payloadSize 10
static BrainMsg BrainMsgRecv;
static char payload[payloadSize];
static uint8 wifly;

//wifly is 1 the brain msg is received via wifly, 0 through i2c
void setBrainData(char* msg, uint8 uart){
    BrainMsg* tempBrain = unpackBrainMsg(msg); //just a cast but this adds clarification

    BrainMsgRecv.flags = tempBrain->flags;
    BrainMsgRecv.parameters = tempBrain->parameters;
    BrainMsgRecv.messageid = tempBrain->messageid;
    BrainMsgRecv.checksum = tempBrain->checksum;
    BrainMsgRecv.payloadLen = tempBrain->payloadLen;
    int i = 0;
    for(i; i < BrainMsgRecv.payloadLen; i++){
        payload[i] = *(tempBrain->payload + i);
    }
    wifly = uart;
}

void sendResponse(){
    switch(BrainMsgRecv.flags){
        case 0x02:
            if((BrainMsgRecv.parameters == 0x05) || (BrainMsgRecv.parameters == 0x06)){
                //sendMotorData();
            }
            else{
                sendMotorAckResponse(BrainMsgRecv.parameters, wifly);
            }
            break;
        default:
            break;
    }
}

void sendData(char* outbuf, uint8 buflen, uint8 wifly){
    if(wifly){
        uart_send_array(outbuf, buflen);
    }
    else{
#ifndef MASTER_PIC
        start_i2c_slave_reply(buflen, outbuf);
#endif
    }
}