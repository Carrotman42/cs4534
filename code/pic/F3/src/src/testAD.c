#include "maindefs.h"
#ifndef SENSOR_PIC
#include "common.h"

#include "testAD.h"
#include "messages.h"
#include "debug.h"
uint8 timesreq = 0;
uint8 sample1 = 0x01;
uint8 sample2 = 0x05;
uint8 sample3 = 0x10;
uint8 sample4 = 0xd8;
uint8 sample5 = 0xfe;

/*
 * returns number of samples  that was filled in the buffer
 */
void reqADData(){
    sensorADData buf[10];
    uint8 numSamples = 0;
    switch(timesreq){
        case 0:
            buf[0].data = sample1;
            numSamples = 1;
            break;
        case 1:
            buf[0].data = sample2;
            buf[1].data = sample3;
            numSamples = 2;
            break;
        case 2:
            buf[0].data = sample4;
            numSamples = 1;
            break;
        case 3:
            buf[0].data = sample5;
            numSamples = 1;
            break;
    }
    timesreq++;
    timesreq = timesreq %4;
    ToMainHigh_sendmsg(numSamples, MSGT_AD, (char*) buf);
}
#endif