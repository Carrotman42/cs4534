#include "comm.h"

#define payloadSize 10
static BrainMsg BrainMsgRecv;
static char payload[payloadSize];
static uint8 wifly;
static uint8 framesRequested;

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

//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(){
    switch(BrainMsgRecv.flags){
        case MOTOR_COMMANDS:
#ifdef MOTOR_PIC
            if(BrainMsgRecv.parameters == 0x05){ // this will only be called on the MOTOR PIC (M->Mo)
                sendEncoderData();
                return 0;
            }
#endif
            if(sendMotorAckResponse(BrainMsgRecv.parameters, wifly)){
#ifdef MOTOR_PIC
                return 0; //motor pic doesn't need to pass anything forward on an ack
#endif
                return MOTOR_ADDR;
            }
            else{
                return 0;
            }
        default:
            break;
    }
    return 0;
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


void handleCommand(){
    uint8 addr = sendResponse();
    if(BrainMsgRecv.flags == HIGH_LEVEL_COMMANDS){
#ifdef MASTER_PIC
        switch(BrainMsgRecv.parameters){
            case 0x00:
                framesRequested = 1;
                break;
            case 0x01:
                break;
            case 0x02:
                break;
            case 0x03:
                framesRequested = 0;
                break;
            default:
                break;
        }
#elif defined(PICMAN)
        
#endif

    }
}