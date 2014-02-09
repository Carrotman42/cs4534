#include "../../../../common/common.h"

#ifdef PIC

#include "sensorcomm.h"
#include "../../../../common/sensor_types.h"
#include "../../../../common/common.h"
#include "testAD.h"
#include "debug.h"
#include "my_i2c.h"


static BrainMsg *BrainMsgRecv;

void setBrainReqData(char* buf){
    /*BrainMsgRecv = (BrainMsg*) buf;
    setDBG(DBG1);
    resetDBG(DBG1);*/
    BrainMsgRecv = unpackBrainMsg(buf); //just a cast but this adds clarification
}

void sendRequestedData(){
    if(BrainMsgRecv->flags == SENSOR_REQ){ //Requesting sensor data
        if(BrainMsgRecv->sensorMask == sensorADid){ //requesting A/D converter data
            //setDBG(DBG2);
            sensorADData data[10];
            uint8 len = reqADData(data);
            /*char dat[1];
            dat[0] = 0x0c;
            start_i2c_slave_reply(len, (char*)data);*/
            sendADdata(data, len);
            //resetDBG(DBG2);
        }
    }
}

void sendADdata(sensorADData*data, int len) {
	char outBuff[100]; //sizeof(RoverMsg) + sizeof(sensorADData) * len
        int bytes_packed = packADData(data, len, outBuff, 100);
        if(bytes_packed == 0){
            //error
        }
        else{
            start_i2c_slave_reply(bytes_packed, outBuff);
        }
}

#endif //PIC