#include "maindefs.h"
#ifndef __XC8
#include <usart.h>
#else
#include <plib/usart.h>
#include <stdint.h>
#endif
#include "my_uart.h"
#include "debug.h"
#include "brain_rover.h"
#include "interrupts.h"



static uart_comm *uc_ptr;
static unsigned char payload_length;
static unsigned char checksum_recv_value;
static unsigned char checksum_calc_value;
const char endmsg[] = "**HELLO*";
unsigned char wifly_setup = 0;

void uart_recv_wifly_debug_handler(){

#ifdef __USE18F46J50
    if (DataRdy1USART()) {
        unsigned char last = Read1USART();
#else
    if (DataRdyUSART()) {
        unsigned char last = ReadUSART();
#endif
        static unsigned char cur = 0;
        if (last == endmsg[cur]) {
            cur++;
            if(cur >= sizeof endmsg - 1) { //-1 for null terminated
                wifly_setup = 1;
            }
        }
        else{
            cur = 0;
        }
    }
}

void uart_recv_int_handler() {
#ifdef __USE18F26J50
    if (DataRdy1USART()) {
        uc_ptr->buffer[uc_ptr->buflen] = Read1USART();
#else
#ifdef __USE18F46J50
    if (DataRdy1USART()) {
        unsigned char recv = Read1USART();
#else
    if (DataRdyUSART()) {
        unsigned char recv = ReadUSART();

#endif
#endif

        int pos = uc_ptr->buflen++;

        uc_ptr->buffer[pos] = recv;
        //We recieved the last byte of data
        //Check the 5th byte recieved for payload length
        if(pos == PAYLOADLEN_POS){
            payload_length = recv;
        }
        // Get checksum byte
        if(pos == CHECKSUM_POS){
            checksum_recv_value = recv;
        }
        // Count any other byte other than checksum
        else{
            checksum_calc_value += recv;
        }
        // check if a message should be sent
        if (pos == payload_length+HEADER_MEMBERS-1){
            pos++;
            if(checksum_calc_value == checksum_recv_value){
                FromUARTInt_sendmsg(pos, MSGT_UART_DATA, (void *) uc_ptr->buffer);
            }
            else{ //Invalid Checksum
                FromUARTInt_sendmsg(pos, MSGT_UART_RECV_FAILED, (void *) uc_ptr->buffer);
            }
            //Clean up for next packet
            uc_ptr->buflen = 0;
            payload_length = 0;
            checksum_recv_value = 0;
            checksum_calc_value = 0;
#ifdef __USE18F46J50
            //Read1USART();    // clears buffer and returns value to nothing
#else
            //ReadUSART();
#endif
        }
        // portion of the bytes were received or there was a corrupt byte or there was 
        // an overflow transmitted to the buffer
        else if (pos >= MAXUARTBUF)
        {
            FromUARTInt_sendmsg(pos, MSGT_OVERRUN, (void *) uc_ptr->buffer);
            uc_ptr->buflen = 0;
            payload_length = 0;
            checksum_recv_value = 0;
            checksum_calc_value = 0;
        }

    }
#ifdef __USE18F26J50
    if (USART1_Status.OVERRUN_ERROR == 1) {
#else
#ifdef __USE18F46J50
    if (USART1_Status.OVERRUN_ERROR == 1) {
#else
    if (USART_Status.OVERRUN_ERROR == 1) {
#endif
#endif
        // we've overrun the USART and must reset
        // send an error message for this
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        FromUARTInt_sendmsg(0, MSGT_OVERRUN, (void *) 0);
    }
#ifdef __USE18F46J50
    if (USART1_Status.FRAME_ERROR) {
#else 
    if (USART_Status.FRAME_ERROR) {
#endif
        init_uart_recv(uc_ptr);
    }
}

void init_uart_recv(uart_comm *uc) {
    uc_ptr->status = UART_IDLE;
    uc_ptr = uc;
    uc_ptr->buflen = 0;
    payload_length = 0;
    checksum_recv_value = 0;
    checksum_calc_value = 0;
}

void uart_send_array(char* data, char length) {
#if !defined(SENSOR_PIC) && !defined(MOTOR_PIC)
    if(!wifly_setup) return; //just return
#endif
    if(uc_ptr->status != UART_IDLE){
        if(in_main()){
            FromMainHigh_sendmsg(length, MSGT_UART_TX_BUSY, data);
        }
        else{
            FromUARTInt_sendmsg(length, MSGT_UART_TX_BUSY, data);
        }
        return;
    }
    uc_ptr->status = UART_TX;
    //TODO: Create logic to prevent you from overriding the current buffer if
    //it has not yet been sent. 
    uint8_t i;
    for(i = 0; i<length; i++) {
        uc_ptr->outBuff[i] = *(data + i);
    }
    uc_ptr->outLength = length;
    uc_ptr->outIndex = 1;
#ifdef __USE18F46J50
    Write1USART(uc_ptr->outBuff[0]);
#else
    WriteUSART(uc_ptr->outBuff[0]);
#endif
    PIR1bits.TXIF = 0;
    PIE1bits.TXIE = 1;
}
//Used to send multiple char. Will get called when uart_send_array is called
//Brian says DONT DELETE! I will find you.
void uart_send_int_handler() {
    if(uc_ptr->outLength > uc_ptr->outIndex){
        uart_send(uc_ptr->outBuff[uc_ptr->outIndex++]);
    }
    else if(uc_ptr->outLength == uc_ptr->outIndex){
        uc_ptr->outIndex++; //still need to increment
    }
    else //uc_ptr->outLength < uc_ptr->outIndex
    {
        // done sending message
        PIE1bits.TXIE = 0;
        PIR1bits.TXIF = 0;
        uc_ptr->status = UART_IDLE;

    }
}

void uart_send(char data){
    //TODO possibly create logic to (without using a while) prevent writing if the buffer is not
    //clear
#ifdef __USE18F46J50
    Write1USART(data);
#else
    WriteUSART(data);
#endif
}