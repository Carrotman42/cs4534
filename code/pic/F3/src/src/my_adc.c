#include "maindefs.h"
#ifndef __XC8
#include <adc.h>
#else
#include <plib/adc.h>
#endif
#include "my_adc.h"
#include "messages.h"
#include "debug.h"


static char ADCBuffer[5];
static char count = 0;

static void addBuffer(char data){
    if(count < 7)
        ADCBuffer[count++] = data;
}

void init_adc(){
    OpenADC(ADC_FOSC_2 & ADC_RIGHT_JUST & ADC_0_TAD,
            ADC_CH1 & ADC_INT_ON & ADC_VREFPLUS_EXT & ADC_VREFMINUS_EXT,
            0xC);

    //Setting AN1 as input
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    /*
    // Configure ADC
    ADCON0 = 0x04;
    ADCON1 = 0x20;
    ADCON2 = 0x80;
    ADCON0 = ADCON0 | 0x01;
    // Configure ADC Interrupts*/
    ADC_INT_ENABLE();
    //INTCONbits.GIE = 1; // Enable Global Interrupts

    //PIE1 = PIE1 | 0x40;  // OR PIE1 with bit_6 - Enables ADC Interrupt
    //INTCON = INTCON | 0x80; // Enable Global Interrupts

}

void adc_int_handler() {
    unsigned int data;
    //readNum(1);
    data = ReadADC();
    data >>= 2;

    //if(data != 0xFF){
        //debugNum(2);
        addBuffer((char) data);
        if(count >= 7){
            ToMainHigh_sendmsg(7,MSGT_AD, ADCBuffer);
            count = 0;
        }
    //}
}
