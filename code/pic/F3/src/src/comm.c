#include "comm.h"

#define payloadSize 10
static BrainMsg LPBrainMsgRecv;
static RoverMsg LPRoverMsgRecv;
static char  LPBrainPayload[payloadSize];
static char  LPRoverPayload[payloadSize];
static BrainMsg HPBrainMsgRecv;
static RoverMsg HPRoverMsgRecv;
static char  HPBrainPayload[payloadSize];
static char  HPRoverPayload[payloadSize];
static uint8 wifly;

static void setData(Msg* staticMsg, char* payload, char* incomingMsg){
    clearHighPriority(incomingMsg); //at this point, hp is useless since the hp functions are already getting called
    BrainMsg* msg = unpackBrainMsg(incomingMsg); //just a cast but this adds clarification
    staticMsg->flags = msg->flags;
    staticMsg->parameters = msg->parameters;
    staticMsg->messageid = msg->messageid;
    staticMsg->checksum = msg->checksum;
    staticMsg->payloadLen = msg->payloadLen;
    int i = 0;
    for(i; (i < staticMsg->payloadLen) && (i < payloadSize) ; i++){
        payload[i] = *(msg->payload + i);
    }
}
//wifly is 1 the brain msg is received via wifly, 0 through i2c
void setBrainDataHP(char* msg){
    setData(&HPBrainMsgRecv, &HPBrainPayload, msg);
}

void setBrainDataLP(char* msg){
    setData(&LPBrainMsgRecv, &LPBrainPayload, msg);
}

void setRoverDataHP(char* msg){
    setData(&HPRoverMsgRecv, &HPRoverPayload, msg);
}

void setRoverDataLP(char* msg){
    setData(&LPRoverMsgRecv, &LPRoverPayload, msg);
}


#ifdef MASTER_PIC
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    switch(brain->flags){
        case MOTOR_COMMANDS:
            if(sendMotorAckResponse(brain->parameters, brain->messageid, wifly)){
                return MOTOR_ADDR;
            }
            return 0;
        case HIGH_LEVEL_COMMANDS:
            sendHighLevelAckResponse(brain->parameters, brain->messageid, wifly);
            break;
        default:
            break;
    }
    return 0;
}
#elif defined(MOTOR_PIC)
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    switch(brain->flags){
        case MOTOR_COMMANDS:
            if(brain->parameters == 0x05){ // this will only be called on the MOTOR PIC (M->Mo)
                sendEncoderData(brain->messageid);
            }
            else{
                sendMotorAckResponse(brain->parameters, brain->messageid, wifly);
            }
            break;
        case HIGH_LEVEL_COMMANDS:
            if(brain->parameters == 0x05){ // this will only be called on the MOTOR PIC (M->Mo)
                static uint8 ack = 0;
                if(!ack){
                    char command[6];
                    uint8 length = generateTurnCompleteNack(command, sizeof command, brain->messageid);
                    makeHighPriority(command);
                    start_i2c_slave_reply(length, command);
                    ack = 1;
                }
                else{
                    char command[6];
                    uint8 length = generateTurnCompleteAck(command, sizeof command, brain->messageid);
                    makeHighPriority(command);
                    start_i2c_slave_reply(length, command);
                    ack = 0;
                }
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
uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    switch(brain->flags){
        case SENSOR_COMMANDS:{
            if(brain->parameters == 0x01){ // this will only be called on the MOTOR PIC (M->Mo)
               sendSensorFrame(brain->messageid);
            }
            else{
                char errorbuf[6];
                uint8 length = generateUnknownCommandError(errorbuf, sizeof errorbuf, wifly);
                sendData(errorbuf, length, wifly);
            }
            break;
        };
        default:{
            char errorbuf[6];
            uint8 length = generateUnknownCommandError(errorbuf, sizeof errorbuf, wifly);
            sendData(errorbuf, length, wifly);
            break;
        };
    }

    return 0;
}
#endif

void sendData(char* outbuf, uint8 buflen, uint8 wifly){
    //uart_send_array(outbuf, buflen);
    if(wifly){
        uart_send_array(outbuf, buflen);
    }
    else{
#ifndef I2C_MASTER
        start_i2c_slave_reply(buflen, outbuf);
#endif
    }
}



static void handleMessage(BrainMsg* brain, char* payload, uint8 source, uint8 dest){
    uint8 addr = sendResponse(brain, source); //either sends ack or data
    //if an ack was sent(e.g. Motor start forward), it can be handled here
    //data would have already been returned in the send response method
#if defined(MASTER_PIC) || defined(PICMAN) || defined(ROVER_EMU)
    propogateCommand(brain, payload, addr, dest);
#endif
}

void handleMessageHP(uint8 source, uint8 dest){
    handleMessage(&HPBrainMsgRecv, HPBrainPayload, source, dest);
}

void handleMessageLP(uint8 source, uint8 dest){
    handleMessage(&LPBrainMsgRecv, LPBrainPayload, source, dest);
}


#ifdef MASTER_PIC
static void propogateCommand(BrainMsg* brain, char* payload, uint8 addr, uint8 dest){
    switch(brain->flags){
        case HIGH_LEVEL_COMMANDS:
            switch(brain->parameters){
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
                switch(brain->parameters){
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
                        length = repackBrainMsg(brain, payload, command, sizeof command, dest);
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
#elif defined(ROVER_EMU)
static void propogateCommand(BrainMsg* brain, char* payload, uint8 addr, uint8 dest){
    switch(brain->flags){
        case HIGH_LEVEL_COMMANDS:
            switch(brain->parameters){
                case 0x00:
                    startFrames();
                    break;
                case 0x03:
                    stopFrames();
                    break;
                default:
                    break;
            }
            break;
        case MOTOR_COMMANDS:
            if((brain->parameters == 0x03) || (brain->parameters == 0x04)){
                char command[6];
                uint8 length = 0;
                length = generateTurnCompleteReq(command, sizeof command, UART_COMM); //tell picman turn complete
                uart_send_array(command, length);
            }
            break;
        default:
            break;

    }
}

uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    switch(brain->flags){
        case MOTOR_COMMANDS:
            if(sendMotorAckResponse(brain->parameters, brain->messageid, wifly)){
                return MOTOR_ADDR;
            }
            return 0;
        case HIGH_LEVEL_COMMANDS:
            sendHighLevelAckResponse(brain->parameters, brain->messageid, wifly);
            break;
        default:
            break;
    }
    return 0;

}
void sendHighLevelAckResponse(uint8 parameters, uint8 messageid, uint8 wifly){
    char outbuf[10];

    uint8 bytes_packed = 0;
    switch(parameters){
        case 0x00:
            bytes_packed = packStartFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x03:
            bytes_packed = packStopFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        default:
            break;
    }
    sendData(outbuf, bytes_packed, wifly);
}

void handleRoverDataHP(){
}

void handleRoverDataLP(){
}

#endif


#if defined(MASTER_PIC)
static void handleRoverData(RoverMsg* rover, char* payload){
    switch(rover->flags){
        case SENSOR_COMMANDS:
            switch(rover->parameters){
                case 0x01:
                    addSensorFrame(payload[0], payload[1], payload[2]);
                    //debugNum(8);
                    break;
                default:
                    //all other cases get an ack
                    break;
            }
        case MOTOR_COMMANDS:
            switch(rover->parameters){
                case 0x05:
                    addEncoderData(payload[0], payload[1], payload[2], payload[3]);
                    //debugNum(8);
                    break;
                default:
                    //all other cases get an ack
                    break;
            }
            break;
        case HIGH_LEVEL_COMMANDS:
            switch(rover->parameters){
                case 0x05:{//ack or nack back from turn complete
                    char command[HEADER_MEMBERS] = "";
                    uint8 length = 0;
                    if(payload[0] == 0){ //nack
                        length = generateTurnCompleteReq(command, sizeof command, I2C_COMM); //ask again
                        i2c_master_send(MOTOR_ADDR, length, command);
                    }
                    else{//ack, here is where I would do error checking and send a command to fix turn by x degrees
                        //for now, just tell picman that the turn is complete.
                        length = generateTurnCompleteReq(command, sizeof command, UART_COMM); //tell picman turn complete
                        uart_send_array(command, length);
                        turnCompleted();
                    }
                    break;
                default:
                    //all other cases get an ack
                    break;
                };
            }
            break;
        case (ACK_FLAG | MOTOR_COMMANDS):
            switch(rover->parameters){
                case 0x03: //one of the turns has been ack'd
                case 0x04:{
                    turnStarted();
                    char command[HEADER_MEMBERS] = "";
                    uint8 length = 0;
                    length = generateTurnCompleteReq(command, sizeof command, I2C_COMM); //ask again
                    //uart_send_array(command, length);
                    i2c_master_send(MOTOR_ADDR, length, command);
                    break;
                };
                default:
                    break;
            }
            break;
        default:
            break;
    }
    //debugNum(1);
    if(frameDataReady()){
        sendFrameData();
        clearFrameData();
    }
}

void handleRoverDataHP(){
    handleRoverData(&HPRoverMsgRecv, HPRoverPayload);
}

void handleRoverDataLP(){
    handleRoverData(&LPRoverMsgRecv, LPRoverPayload);
}

void sendHighLevelAckResponse(uint8 parameters, uint8 messageid, uint8 wifly){
    char outbuf[10];

    uint8 bytes_packed = 0;
    switch(parameters){
        case 0x00:
            bytes_packed = packStartFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x01:
            bytes_packed = packFrameDataAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x03:
            bytes_packed = packStopFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        default:
            break;
    }
    sendData(outbuf, bytes_packed, wifly);
}
#elif defined(ARM_EMU)
uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    return 0; //arm emu doesn't send repsonses
}

static void handleRoverData(RoverMsg* rover, char* payload){
    debugNum(1);
    switch(rover->flags){
        case HIGH_LEVEL_COMMANDS:
            switch(rover->parameters){
                case 0x04:
                    colorSensorTriggered();
                    break;
                case 0x05:{//ack or nack back from turn complete
                    char command[HEADER_MEMBERS] = "";
                    uint8 length = 0;
                    if(payload[0] == 0){ //nack
                        length = generateTurnCompleteReq(command, sizeof command, I2C_COMM); //ask again
                        i2c_master_send(PICMAN_ADDR, length, command);
                    }
                    else{
                        turnCompleted();
                    }
                    break;
                default:
                    //all other cases get an ack
                    break;
                };
            }
            break;
        case (ACK_FLAG | MOTOR_COMMANDS):
            switch(rover->parameters){
                case 0x03: //one of the turns has been ack'd
                case 0x04:{
                    turnStarted();
                    char command[HEADER_MEMBERS] = "";
                    uint8 length = 0;
                    length = generateTurnCompleteReq(command, sizeof command, I2C_COMM); //ask again
                    //uart_send_array(command, length);
                    i2c_master_send(PICMAN_ADDR, length, command);
                    break;
                };
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void handleRoverDataHP(){
    handleRoverData(&HPRoverMsgRecv, HPRoverPayload);
}

void handleRoverDataLP(){
    handleRoverData(&LPRoverMsgRecv, LPRoverPayload);
}

void sendHighLevelAckResponse(uint8 parameters, uint8 messageid, uint8 wifly){
    return; //no acks for armu emu
}
#elif defined(PICMAN)
//the return value dictates whether or not the information needs to be passed to the slave PICs
//0 is "no information needs to be passed"
//Otherwise, the addres is returned
uint8 sendResponse(BrainMsg* brain, uint8 wifly){
    if(isColorSensorTriggered()){
        char command[5];
        uint8 length = generateColorSensorSensed(command, sizeof command, I2C_COMM);
        sendData(command, length, I2C_COMM); //just send the color sensor sensed no matter what command was sent to the rover, we won't care
        return 0;
    }
    switch(brain->flags){
        case MOTOR_COMMANDS:
            if(sendMotorAckResponse(brain->parameters, brain->messageid, wifly)){
                return MOTOR_ADDR;
            }
            return 0;
        case HIGH_LEVEL_COMMANDS:
            switch(brain->parameters){
                case 0x02:
                    if(!frameDataReady()){
                        addSensorFrame(0,0,0);
                        addEncoderData(0,0,0,0); //send all 0's if the frame isn't ready
                        clearFrameData(); //resets flags for frame data ready
                    }
                    sendFrameData();
                    break;
                case 0x05:{
                    char command[6];
                    uint8 length = 0;
                    if(!isTurnComplete()){
                        length = generateTurnCompleteNack(command, sizeof command, brain->messageid);
                    }
                    else{
                        length = generateTurnCompleteAck(command, sizeof command, brain->messageid);
                    }
                    makeHighPriority(command);
                    start_i2c_slave_reply(length, command);
                    break;
                };
                default:
                    sendHighLevelAckResponse(brain->parameters, brain->messageid, wifly);
                    break;
            }
            break;
        default:
            break;
    }
    return 0;
}

//the picman doesn't care about the address
//it sends most commands across uart.
//only command it won't propogate is read frames
static void propogateCommand(BrainMsg* brain, char* payload, uint8 addr, uint8 dest){
    if(isColorSensorTriggered()){
        return; //don't care about propogating any commands
    }
    char command[6];
    uint8 length = 0;
    switch(brain->flags){
        case HIGH_LEVEL_COMMANDS:
            switch(brain->parameters){
                case 0x00:
                case 0x03://start and stop frames are just packaged up and sent to master pic
                    length = repackBrainMsg(brain, payload, command, sizeof command, dest);
                    break;
                default:
                    break;
            }
            break;
        case MOTOR_COMMANDS:
            if(addr == MOTOR_ADDR){
                switch(brain->parameters){
                    case 0x00:
                    case 0x01:
                    case 0x02:
                    case 0x03:
                    case 0x04:
                        length = repackBrainMsg(brain, payload, command, sizeof command, dest);
                        break;
                    default:
                        break;
                }
            }
            break;
    }
    if(length != 0){
        uart_send_array(command, length);
    }
}

static void handleRoverData(RoverMsg* rover, char* payload){
    switch(rover->flags){
        case HIGH_LEVEL_COMMANDS:
            switch(rover->parameters){
                case 0x01:
                    addSensorFrame(payload[0], payload[1], payload[2]);
                    addEncoderData(payload[3], payload[4], payload[5], payload[6]);
                    break;
                case 0x04:
                    colorSensorTriggered();
                    char command[5];
                    uint8 length = generateStop(command, sizeof command, UART_COMM);
                    sendData(command, length, UART_COMM); //send stop to stop the rover (more important than frames)
                    length = generateStopFrames(command, sizeof command, UART_COMM);
                    sendData(command, length, UART_COMM); //want to send stop frames because the arm no longer cares about the data
                    break;
                case 0x05:
                    turnCompleted();
                    break;
                default:
                    break;
            }
        default:
            break;
    }
}

void handleRoverDataHP(){
    handleRoverData(&HPRoverMsgRecv, HPRoverPayload);
}

void handleRoverDataLP(){
    handleRoverData(&LPRoverMsgRecv, LPRoverPayload);
}

void sendHighLevelAckResponse(uint8 parameters, uint8 messageid, uint8 wifly){
    char outbuf[10];

    uint8 bytes_packed = 0;
    uint8 ack = 1;
    switch(parameters){
        case 0x00:
            bytes_packed = packStartFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x01:
            bytes_packed = packFrameDataAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x03:
            bytes_packed = packStopFramesAck(outbuf, sizeof outbuf, messageid);
            break;
        case 0x04:
            bytes_packed = packColorSensedAck(outbuf, sizeof outbuf, messageid);
            break;
        default:
            ack = 0;
            break;
    }
    if(ack){
        sendData(outbuf, bytes_packed, wifly);
    }
}
#endif