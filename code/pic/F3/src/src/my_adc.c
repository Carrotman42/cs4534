#include "maindefs.h"
//#define ADCCONFIG
#ifdef SENSOR_PIC
#ifndef __XC8
#include <adc.h>
#else
#include <plib/adc.h>
#endif
#include "my_adc.h"
#include "messages.h"
#include "debug.h"

#include "my_uart.h"  // REMOVE AFTER MS4
#include <math.h>

static char ADCBuffer[5];
static char distanceArray[2];
static char chn0Data;
static irBuffer buffer;
static char count = 0;
static char channel = 0;
static char error = 0;

#ifndef ADCCONFIG
static const float ir0_m[7] = {-0.1163,-0.2703,-0.7143,-0.9091,-1.1111,-10.0000,-3.3333};
static const float ir0_b[7] = {30.9302,45.4054,70.7143,79.0909,85.5556,290.0000,143.3333};
static const float ir1_m[7] = {-0.1235,-0.2857,-0.5882,-1.0000,-1.1111,-1.6667,-5.0000};
static const float ir1_b[7] = {31.3580,46.2857,63.5294,80.0000,83.3333,95.0000,145.0000};

static const char ir0_voltageValues[8] = {180,94,57,43,32,23,21,19};
static const char ir1_voltageValues[8] = {173,92,57,40,30,21,15,13};

//KEEP
//static const float ir0_m[7] = {-.1163,-.333,-.666,-1,-1.176,1.666,-.606};
//static const float ir0_b[7] = {31.279,52.333,74.666,92,99.411,4.166,93.939};
//static const float ir1_m[7] = {-.1333,-.3448,-.7407,-.9091,-1.666,-4,-.9091};
//static const float ir1_b[7] = {32.933,53.448,80.37,89.5455,122.5,210,101.8182};
//
//static const char ir0_voltageValues[8] = {183,97,67,52,42,38,34,23};
//static const char ir1_voltageValues[8] = {172,97,68,55,44,38,35,24};

static uint8 ir0_distance = 0;
static uint8 ir1_distance = 0;
#endif

static void addBuffer(char data){
    if(count < 7)
        ADCBuffer[count++] = data;
}

void addDataToBuffer(char ir0Data, char ir1Data){
//    debugNum(2);
    buffer.ir0Array[buffer.count] = ir0Data;
    buffer.ir1Array[buffer.count++] = ir1Data;

    if(count == IRBUFFERSIZE)
        buffer.count = 0;

//    sort(buffer.ir0Array);
//    sort(buffer.ir1Array);
}

void sort(uint8* array){
//    debugNum(1);
    char t = 0;
    for (char i = 0; i < IRBUFFERSIZE-1; i++){
        for (char j = 0; j < IRBUFFERSIZE-i-1;j++){
            if (array[j] > array[j+1]){
                t = array[j];
                array[j] = array[j+1];
                array[j+1] = t;
            }
        }
    }
//    debugNum(1);
}

void init_adc(){
//    OpenADC(ADC_FOSC_2 & ADC_RIGHT_JUST & ADC_0_TAD,
//            ADC_CH0 & ADC_CH1 & ADC_INT_ON & ADC_VREFPLUS_EXT & ADC_VREFMINUS_EXT,
//            0xC);

    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 0;

    ADCON1bits.VCFG1 = 1;
    ADCON1bits.VCFG0 = 1;
    ADCON1bits.PCFG = 0xC;

    ADCON2bits.ADFM = 1;
    ADCON2bits.ACQT = 0b001;
    ADCON2bits.ADCS = 0;

//    INTCONbits.GIE = 1;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;

    //Setting AN0, AN1 as input
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;

//    for(char i = 0; i < IRBUFFERSIZE; i++){
//        buffer.ir0Array[i] = 0xFF;
//        buffer.ir1Array[i] = 0xFF;
//    }
    buffer.count = 0;

    ADCON0bits.ADON = 1;

    /*
    // Configure ADC
    ADCON0 = 0x04;
    ADCON1 = 0x20;
    ADCON2 = 0x80;
    ADCON0 = ADCON0 | 0x01;
    // Configure ADC Interrupts*/
//    ADC_INT_ENABLE();
    //INTCONbits.GIE = 1; // Enable Global Interrupts

    //PIE1 = PIE1 | 0x40;  // OR PIE1 with bit_6 - Enables ADC Interrupt
    //INTCON = INTCON | 0x80; // Enable Global Interrupts

}

void adc_int_handler() {
    uint8 data;
    //readNum(1);
//    debugNum(4);
    data = ReadADC()>>2;
//    data >>= 2;

    //if(data != 0xFF){
//        addBuffer((char) data);
//        if(count >= 7){
//            ToMainHigh_sendmsg(7,MSGT_AD, ADCBuffer);
//            count = 0;
//        }
    //}

    if(channel == 0){
//        dataArray[0] = data;
        chn0Data = (char) data;
        ADCON0bits.CHS = 1;
        channel = 1;
        ADCON0bits.GO = 1;
//        debugNum(1);
    }
    else if(channel == 1){
        addDataToBuffer(chn0Data, (char) data);
//        distanceArray[0] = buffer.ir0Array[HALFBUFFER];
//        distanceArray[1] = buffer.ir1Array[HALFBUFFER];
//        uart_send_array(distanceArray, 2);
//        uart_send(buffer.ir0Array[HALFBUFFER]);
//        uart_send(buffer.ir1Array[HALFBUFFER]);

        ADCON0bits.CHS = 0;
        channel = 0;
//        debugNum(2);
//        debugNum(2);
    }
}

char* transmitData(){
    char ir0[IRBUFFERSIZE] = {0};
    char ir1[IRBUFFERSIZE] = {0};

    for(char i = 0; i < IRBUFFERSIZE; i++){
        ir0[i] = buffer.ir0Array[i];
        ir1[i] = buffer.ir1Array[i];
    }

    sort(ir0);
    sort(ir1);

#ifndef ADCCONFIG
    calculateDistance(ir0[HALFBUFFER],ir1[HALFBUFFER]);

    distanceArray[0] = ir0_distance;
    distanceArray[1] = ir1_distance;

    //uart_send_array(distanceArray,2);
    return distanceArray;

#endif
#ifdef ADCCONFIG
    uart_send((char) ir0[HALFBUFFER]);
    uart_send((char) ir1[HALFBUFFER]);
#endif
}

#ifndef ADCCONFIG
void calculateDistance(char ir0_rawData, char ir1_rawData){
    uint8 ir0_index;
    uint8 ir1_index;

    if(ir0_rawData >= ir0_voltageValues[0])
        ir0_index = 0;
    else if(ir0_rawData >= ir0_voltageValues[1])
        ir0_index = 1;
    else if(ir0_rawData >= ir0_voltageValues[2])
        ir0_index = 2;
    else if(ir0_rawData >= ir0_voltageValues[3])
        ir0_index = 3;
    else if(ir0_rawData >= ir0_voltageValues[4])
        ir0_index = 4;
    else if(ir0_rawData >= ir0_voltageValues[5])
        ir0_index = 5;
    else if(ir0_rawData >= ir0_voltageValues[6])
        ir0_index = 6;
    else if(ir0_rawData >= ir0_voltageValues[7])
        ir0_index = 7;

    if(ir1_rawData >= ir1_voltageValues[0])
        ir1_index = 0;
    else if(ir1_rawData >= ir1_voltageValues[1])
        ir1_index = 1;
    else if(ir1_rawData >= ir1_voltageValues[2])
        ir1_index = 2;
    else if(ir1_rawData >= ir1_voltageValues[3])
        ir1_index = 3;
    else if(ir1_rawData >= ir1_voltageValues[4])
        ir1_index = 4;
    else if(ir1_rawData >= ir1_voltageValues[5])
        ir1_index = 5;
    else if(ir1_rawData >= ir1_voltageValues[6])
        ir1_index = 6;
    else if(ir1_rawData >= ir1_voltageValues[7])
        ir1_index = 7;


    ir0_distance = (uint8) ((ir0_m[ir0_index] * ir0_rawData) + ir0_b[ir0_index]);
    ir1_distance = (uint8) ((ir1_m[ir1_index] * ir1_rawData) + ir1_b[ir1_index]);

    //Convert ADC to voltage value
//    float ir0_voltage = ((((unsigned int) ir0_rawData)<<2)/1024)*3.4+0;
//    float ir1_voltage = ((((unsigned int) ir1_rawData)<<2)/1024)*3.4+0;
//
//    //Using equation suggested on sparkfun
//    ir0_distance = (int) 41.543 * pow((ir0_voltage + 0.30221),-1.5281);
//    ir1_distance = (int) 41.543 * pow((ir1_voltage + 0.30221),-1.5281);
//    debugNum(4);

//    error = 0;
//    float ir0_cu = ir0_rawData*ir0_rawData*ir0_rawData;
//    float ir0_sq = ir0_rawData*ir0_rawData;
//    float ir1_cu = ir1_rawData*ir1_rawData*ir1_rawData;
//    float ir1_sq = ir1_rawData*ir1_rawData;
//
//    //Using cubic polynomial fits
//    if(ir0_rawData < 13)
//        error = -1;
//    else if(ir0_rawData < 15)
//        ir0_distance = 0.36*ir0_cu - 0.3*ir0_sq - 5.83*ir0_rawData + 80;
//    else if(ir0_rawData < 21)
//        ir0_distance = -0.02*ir0_cu + 0.3*ir0_sq - 2.73*ir0_rawData + 70;
//    else if(ir0_rawData < 30)
//        ir0_distance = 0.05*ir0_sq - 1.35*ir0_rawData + 60;
//    else if(ir0_rawData < 40)
//        ir0_distance =  -0.01*ir0_sq - 1.05*ir0_rawData + 50;
//    else if(ir0_rawData < 57)
//        ir0_distance =  0.01*ir0_sq - 0.76*ir0_rawData + 40;
//    else if(ir0_rawData < 92)
//        ir0_distance =  -0.4*ir0_rawData + 30;
//    else if(ir0_rawData < 173)
//        ir0_distance =  -0.18*ir0_rawData + 20;
//    else
//        error = 1;
//
//    if(ir1_rawData < 13)
//        error = -1;
//    else if(ir1_rawData < 15)
//        ir1_distance = 0.36*ir1_cu - 0.3*ir1_sq - 5.83*ir1_rawData + 80;
//    else if(ir1_rawData < 21)
//        ir1_distance = -0.02*ir1_cu + 0.3*ir1_sq - 2.73*ir1_rawData + 70;
//    else if(ir1_rawData < 30)
//        ir1_distance = 0.05*ir1_sq - 1.35*ir1_rawData + 60;
//    else if(ir1_rawData < 40)
//        ir1_distance =  -0.01*ir1_sq - 1.05*ir1_rawData + 50;
//    else if(ir1_rawData < 57)
//        ir1_distance =  0.01*ir1_sq - 0.76*ir1_rawData + 40;
//    else if(ir1_rawData < 92)
//        ir1_distance =  -0.4*ir1_rawData + 30;
//    else if(ir1_rawData < 173)
//        ir1_distance =  -0.18*ir1_rawData + 20;
//    else
//        error = 1;
}
#endif
#endif