#include "sensorcomm.h"

#include "../../../../common/sensor_types.h"
#include "../../../../common/common.h"
#include "testAD.h"
#include "debug.h"
#include "my_i2c.h"

/*
static BrainMsg BrainMsgRecv;
static sensorADaccumulator ADacc;

void setBrainReqData(char* buf){

    BrainMsg* tempBrain = unpackBrainMsg(buf); //just a cast but this adds clarification

    //reqADData();
    BrainMsgRecv.flags = tempBrain->flags;
    BrainMsgRecv.parameters = tempBrain->parameters;
}

void sendRequestedData(){
    if(BrainMsgRecv.flags == SENSOR_COMMANDS){ //Requesting sensor data
        if(BrainMsgRecv.parameters == sensorADid){ //requesting A/D converter data
            sendADdata();
            resetADacc(); //make sure we don't send extra on next request
        }
    }
}

void sendADdata() {
    //readNum(1);
	char outBuff[MAX_I2C_SENSOR_DATA_LEN + HEADER_MEMBERS]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
        int bytes_packed = packADData( ADacc.data, ADacc.len, outBuff, sizeof(outBuff));
        if(bytes_packed == 0){
            //error
        }
        else{
            //char* dat = (char*) ADacc->data;
            //readNum((int)dat[0]);
            start_i2c_slave_reply(MAX_I2C_SENSOR_DATA_LEN + HEADER_MEMBERS, outBuff);
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
    if(ADacc.len + len < (int) sizeof(ADacc.data)){
        int i = 0;
        for(i; i < len; i++){
            ADacc.data[ADacc.len++] = data[i]; //set the accumulator at position len to the data at i, then increment the accumulator length
        }
    }
    
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
*/