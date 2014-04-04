#include "maindefs.h"
#ifndef __XC8
#include <adc.h>
#else
#include <plib/adc.h>
#endif
#include "my_adc.h"
#include "messages.h"
#include "debug.h"

#ifdef SENSORMS3
#include "my_uart.h"
//#include "my_ultrasonic.h"
static char dataArray[2];
#endif

static char ADCBuffer[5];
static char count = 0;
static char channel = 0;

static void addBuffer(char data){
    if(count < 7)
        ADCBuffer[count++] = data;
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
    data = ReadADC();
    data >>= 2;

#ifdef SENSORMS3
//    char getDistanceUS()
    uart_send((char) data);
#else
    //if(data != 0xFF){
        //debugNum(2);
        addBuffer((char) data);
        if(count >= 7){
            ToMainHigh_sendmsg(7,MSGT_AD, ADCBuffer);
            count = 0;
        }
    //}
#endif
    if(channel == 0){
        ADCON0bits.CHS = 1;
        channel = 1;
        ADCON0bits.GO = 1;
        debugNum(16);
    }
    else if(channel == 1){
        ADCON0bits.CHS = 0;
        channel = 0;
        debugNum(16);
    }
}
