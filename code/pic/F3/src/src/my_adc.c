#include "maindefs.h"
#ifndef __XC8
#include <adc.h>
#else
#include <plib/adc.h>
#endif
#include "my_adc.h"
#include "messages.h"
#include "debug.h"

#include "my_uart.h"  // REMOVE AFTER MS4

static char ADCBuffer[5];
static char distanceArray[2];
static char chn0Data;
static irBuffer buffer;
static char count = 0;
static char channel = 0;

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
    debugNum(1);
}

void init_adc(){
//    OpenADC(ADC_FOSC_2 & ADC_RIGHT_JUST & ADC_0_TAD,
//            ADC_CH1 & ADC_INT_ON & ADC_VREFPLUS_EXT & ADC_VREFMINUS_EXT,
//            0xC);

    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;

    ADCON1bits.VCFG1 = 1;
    ADCON1bits.VCFG0 = 1;
    ADCON1bits.PCFG = 0b1011;

    ADCON2bits.ADFM = 1;
    ADCON2bits.ACQT = 0b010;
    ADCON2bits.ADCS = 0;

    INTCONbits.GIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;

    //Setting AN0, AN1 as input
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;

//    for(char i = 0; i < IRBUFFERSIZE; i++){
//        buffer.ir0Array[i] = 0xFF;
//        buffer.ir1Array[i] = 0xFF;
//    }
    buffer.count = 0;

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
    unsigned int data;
    //readNum(1);
//    debugNum(4);
    data = ReadADC();
    data >>= 2;

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
//        distanceArray[0] = buffer.ir0Array[4];
//        distanceArray[1] = buffer.ir1Array[4];
//        uart_send_array(distanceArray, 2);
//        uart_send(buffer.ir0Array[4]);
//        uart_send(buffer.ir1Array[4]);

        ADCON0bits.CHS = 0;
        channel = 0;
        debugNum(2);
        debugNum(2);
    }
}

void transmitData(){
    char ir0[IRBUFFERSIZE];
    char ir1[IRBUFFERSIZE];

    for(char i = 0; i < IRBUFFERSIZE; i++){
        ir0[i] = buffer.ir0Array[i];
        ir1[i] = buffer.ir1Array[i];
    }

    sort(ir0);
    sort(ir1);

    uart_send((char) ir0[4]);
    uart_send((char) ir1[4]);
}
