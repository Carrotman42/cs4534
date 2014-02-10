#include "maindefs.h"
#ifndef __XC8
#include <adc.h>
#else
#include <plib/adc.h>
#endif
#include "my_adc.h"
#include "messages.h"


void init_adc(){
    //Setting AN1 as input
    TRISAbits.TRISA1 = 1;

    // Configure ADC
    ADCON0 = 0x04;
    ADCON1 = 0x00;
    ADCON2 = 0x00;
    ADCON0 = ADCON0 | 0x01;

    // Configure ADC Interrupts
    PIE1bits.ADIE = 1;  // Enable ADC Interrupt
    INTCONbits.GIE = 1; // Enable Global Interrupts

    //PIE1 = PIE1 | 0x40;  // OR PIE1 with bit_6 - Enables ADC Interrupt
    //INTCON = INTCON | 0x80; // Enable Global Interrupts
}

static char ADCBuffer[5];
static char count = 0;

void adc_int_handler() {
    unsigned int data;

    data = ReadADC();
    data >>= 2;
    addBuffer((char) data);
    if(count >= 5){
        ToMainLow_sendmsg(5,MSGT_AD, ADCBuffer);
        count = 0;
    }
}

static int addBuffer(char data){
    if(count < 5)
        ADCBuffer[count++] = data;
}