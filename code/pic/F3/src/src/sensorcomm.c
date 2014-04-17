#include "sensorcomm.h"

#include "sensor_types.h"
#include "common.h"
#include "debug.h"
#include "my_i2c.h"
#include "my_ultrasonic.h"
#include "my_adc.h"

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

#ifdef SENSOR_PIC
void sendSensorFrame(uint8 msgid){
    addSensorFrame(0x05,0x06,0x07);//will need a function to actually get the sensor data.
                                   //For now, send dummy values
//    char usDistance = getDistanceUS();
//    debugNum(2);
//    char* irDistances = transmitData();
//    debugNum(8);

//    addSensorFrame(usDistance,*(irDistances),*(irDistances+1));

    sendFrameData(msgid);
}
#elif defined(PICMAN) || defined(ARM_EMU)
static uint8 colorSensorStatus = 0;
static uint8 timesColorTriggered = 0;
void colorSensorTriggered(){
    colorSensorStatus = 1;
    timesColorTriggered++;
}
uint8 isColorSensorTriggered(){
    return colorSensorStatus; 
}
void clearColorSensorStatus(){
    colorSensorStatus = 0;
}
uint8 timesColorSensorTriggered(){
    return timesColorTriggered;
}
#endif