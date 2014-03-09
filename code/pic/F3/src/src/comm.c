#include "comm.h"

#define payloadSize 10
static BrainMsg BrainMsgRecv;
static RoverMsg RoverMsgRecv;
static char payload[payloadSize];
static uint8 wifly;

//wifly is 1 the brain msg is received via wifly, 0 through i2c
void setBrainData(char* msg){
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
}

void setRoverData(char* msg){
    BrainMsg* tempRover = unpackBrainMsg(msg); //just a cast but this adds clarification
                                               //BrainMsg is the Same as RoverMsg
    RoverMsgRecv.flags = tempRover->flags;
    RoverMsgRecv.parameters = tempRover->parameters;
    RoverMsgRecv.messageid = tempRover->messageid;
    RoverMsgRecv.checksum = tempRover->checksum;
    RoverMsgRecv.payloadLen = tempRover->payloadLen;
    int i = 0;
    for(i; i < RoverMsgRecv.payloadLen; i++){
        payload[i] = *(tempRover->payload + i);
    }
}

//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
//uint8 sendResponse(uint8 wifly){
//    switch(BrainMsgRecv.flags){
//        case MOTOR_COMMANDS:
//#ifdef MOTOR_PIC
//            if(BrainMsgRecv.parameters == 0x05){ // this will only be called on the MOTOR PIC (M->Mo)
//                sendEncoderData();
//            }
//            else{
//                sendMotorAckResponse(BrainMsgRecv.parameters, BrainMsgRecv.messageid, wifly);
//            }
//            return 0;
//#else
//            if(sendMotorAckResponse(BrainMsgRecv.parameters, BrainMsgRecv.messageid, wifly)){
//                return MOTOR_ADDR;
//            }
//            return 0;
//#endif
//        default:
//            break;
//    }
//    return 0;
//}

#ifdef MASTER_PIC
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(uint8 wifly){
    switch(BrainMsgRecv.flags){
        case MOTOR_COMMANDS:
            if(sendMotorAckResponse(BrainMsgRecv.parameters, BrainMsgRecv.messageid, wifly)){
                return MOTOR_ADDR;
            }
            return 0;
        default:
            break;
    }
    return 0;
}
#elif defined(MOTOR_PIC)
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(uint8 wifly){
    switch(BrainMsgRecv.flags){
        case MOTOR_COMMANDS:
            if(BrainMsgRecv.parameters == 0x05){ // this will only be called on the MOTOR PIC (M->Mo)
                sendEncoderData(BrainMsgRecv.messageid);
            }
            else{
                sendMotorAckResponse(BrainMsgRecv.parameters, BrainMsgRecv.messageid, wifly);
            }
            break;
        default:
        {
            char errorbuf[6];
            uint8 length = generateUnknownCommandError(errorbuf, sizeof errorbuf, wifly);
            start_i2c_slave_reply(length, errorbuf);
            break;
        };
    }

    return 0;
}
#elif defined(SENSOR_PIC)
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(uint8 wifly){
    switch(BrainMsgRecv.flags){
        case SENSOR_COMMANDS:
            if(BrainMsgRecv.parameters == 0x01){ // this will only be called on the MOTOR PIC (M->Mo)
               sendSensorFrame(BrainMsgRecv.messageid);
            }
            break;
        default:
        {
            char errorbuf[6];
            uint8 length = generateUnknownCommandError(errorbuf, sizeof errorbuf, wifly);
            start_i2c_slave_reply(length, errorbuf);
            break;
        };
    }

    return 0;
}
#endif

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



void handleMessage(uint8 source, uint8 dest){
    uint8 addr = sendResponse(source); //either sends ack or data
    //if an ack was sent(e.g. Motor start forward), it can be handled here
    //data would have already been returned in the send response method
#if defined(MASTER_PIC) || defined(PICMAN)
    propogateCommand(addr, dest);
#endif
}


#ifdef MASTER_PIC
static void propogateCommand(uint8 addr, uint8 dest){
    switch(BrainMsgRecv.flags){
        case HIGH_LEVEL_COMMANDS:
            switch(BrainMsgRecv.parameters){
                case 0x00:
                    startFrames();
                    break;
                case 0x01:
                    break;
                case 0x02:
                    break;
                case 0x03:
                    stopFrames();
                    break;
                default:
                    break;
            }
            break;
        case MOTOR_COMMANDS:
            if(addr == MOTOR_ADDR){
                char command[6];
                uint8 length = 0;
                switch(BrainMsgRecv.parameters){
                    case 0x00:
                        //length = generateStartForward(command, sizeof command, dest, BrainMsgRecv.payload[0]);
                        //break;
                    case 0x01:
                        //length = generateStartBackward(command, sizeof command, dest, BrainMsgRecv.payload[0]);
                        //break;
                    case 0x02:
                        //length = generateStop(command, sizeof command, dest);
                        //break;
                    case 0x03:
                        //length = generateTurnCW(command, sizeof command, dest, BrainMsgRecv.payload[0]);
                        //break;
                    case 0x04:
                        //length = generateTurnCCW(command, sizeof command, dest, BrainMsgRecv.payload[0]);
                        length = repackBrainMsg(&BrainMsgRecv, command, sizeof command, dest);
                        break;
                    default:
                        break;
                }
                if(length != 0){
                    i2c_master_send(addr, length, command);
                }
            }
            break;

    }
}

#elif defined(PICMAN)

static void propogateCommand(){

}
#endif


#ifdef MASTER_PIC
void handleRoverData(){
    switch(RoverMsgRecv.flags){
        case SENSOR_COMMANDS:
            switch(RoverMsgRecv.parameters){
                case 0x01:
                    addSensorFrame(RoverMsgRecv.payload[0], RoverMsgRecv.payload[1], RoverMsgRecv.payload[2]);
                    debugNum(3);
                    break;
                default:
                    //all other cases get an ack
                    break;
            }
        case MOTOR_COMMANDS:
            switch(RoverMsgRecv.parameters){
                case 0x05:
                    addEncoderData(RoverMsgRecv.payload[0], RoverMsgRecv.payload[1], RoverMsgRecv.payload[2], RoverMsgRecv.payload[3]);
                    //debugNum(3);
                    break;
                default:
                    //all other cases get an ack
                    break;
            }
            break;
        case HIGH_LEVEL_COMMANDS:
            break;
        default:
            break;
    }
}
#endif