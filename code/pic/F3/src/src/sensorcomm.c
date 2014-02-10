#include "../../../../common/common.h"


#include "sensorcomm.h"
#include "../../../../common/sensor_types.h"
#include "../../../../common/common.h"
#include "testAD.h"
#include "debug.h"
#include "my_i2c.h"
#include "maindefs.h"


static BrainMsg BrainMsgRecv;
static sensorADaccumulator ADacc;

void setBrainReqData(char* buf){

    BrainMsg* tempBrain = unpackBrainMsg(buf); //just a cast but this adds clarification

    if(tempBrain->flags == 0){
        reqADData();
    }
    else{
        BrainMsgRecv.flags = tempBrain->flags;
        BrainMsgRecv.sensorMask = tempBrain->sensorMask;
    }
}

void sendRequestedData(){
    readNum(1);
    if(BrainMsgRecv.flags == SENSOR_REQ){ //Requesting sensor data
        readNum(2);
        if(BrainMsgRecv.sensorMask == sensorADid){ //requesting A/D converter data
            readNum(4);
            sendADdata();
            resetADacc(); //make sure we don't send extra on next request
        }
    }
}

void sendADdata() {
    //readNum(1);
	char outBuff[100]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
        int bytes_packed = packADData( ADacc.data, ADacc.len, outBuff, 100);
        if(bytes_packed == 0){
            //error
        }
        else{
            //char* dat = (char*) ADacc->data;
            //readNum((int)dat[0]);
            start_i2c_slave_reply(bytes_packed, outBuff);
        }
}

void addDataPoints(int sensorid, void* data, int len){
    //switch(sensorid){
    //    case sensorADid:
    if(sensorid == sensorADid){
        addADDataPoints((sensorADData*) data, len);
        //break;
    }
}

void addADDataPoints(sensorADData* data, int len){
    //readNum(len);
    if(ADacc.len + len < MSGLEN){
        int i = 0;
        for(i; i < len; i++){
            ADacc.data[ADacc.len++] = data[i]; //set the accumulator at position len to the data at i, then increment the accumulator length
        }
    }

    //char* dat = (char*) ADacc->data;
    //readNum((int)dat[0]);
    //readNum(ADacc->len);
    
}

void resetADacc(){
    int i = 0;
    for(i; i < ADacc.len; i++){
        ADacc.data[i].data = 0;
    }
    ADacc.len = 0;
}

void resetAccumulators(){
    resetADacc();
}