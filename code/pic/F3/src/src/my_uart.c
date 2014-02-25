#include "maindefs.h"
#ifndef __XC8
#include <usart.h>
#else
#include <plib/usart.h>
#include <stdint.h>
#endif
#include "my_uart.h"
#include "debug.h"
#include "../../../../common/communication/brain_rover.h"



static uart_comm *uc_ptr;
static unsigned char payload_length;
static unsigned char checksum_recv_value;
static unsigned char checksum_calc_value;

void uart_recv_int_handler() {
#ifdef __USE18F26J50
    if (DataRdy1USART()) {
        uc_ptr->buffer[uc_ptr->buflen] = Read1USART();
#else
#ifdef __USE18F46J50
    if (DataRdy1USART()) {
        uc_ptr->buffer[uc_ptr->buflen] = Read1USART();
#else

#endif
#endif

    //debugNum(1);
    if (DataRdyUSART()) {
        uc_ptr->buffer[uc_ptr->buflen] = ReadUSART();
        //We recieved the last byte of data
        uc_ptr->buflen++;
        //Check the 5th byte recieved for payload length
        if(uc_ptr->buflen == HEADER_MEMBERS){
            payload_length = uc_ptr->buffer[HEADER_MEMBERS-1];
        }
        // Get checksum byte
        else if(uc_ptr->buflen == HEADER_MEMBERS-1){
            checksum_recv_value = uc_ptr->buffer[HEADER_MEMBERS-2];
        }
        // Count any other byte other than checksum
        else if(uc_ptr->buflen != HEADER_MEMBERS-1){
            checksum_calc_value += uc_ptr->buffer[uc_ptr->buflen-1];
        }
        // check if a message should be sent
//        if (uc_ptr->buffer[uc_ptr->buflen-1] == '\r') {
        if (uc_ptr->buflen == payload_length){
            if(checksum_calc_value == checksum_recv_value)
                ToMainLow_sendmsg(uc_ptr->buflen, MSGT_UART_DATA, (void *) uc_ptr->buffer);
            else //Invalid Checksum
                ToMainLow_sendmsg(uc_ptr->buflen, MSGT_UART_RECV_FAILED, (void *) uc_ptr->buffer);
            //Clean up for next packet
            uc_ptr->buflen = 0;
            payload_length = 0;
            checksum_recv_value = 0;
            checksum_calc_value = 0;
            ReadUSART();    // clears buffer and returns value to nothing
        }
        // portion of the bytes were received or there was a corrupt byte or there was 
        // an overflow transmitted to the buffer
        else if (uc_ptr->buflen >= MAXUARTBUF)
        {
            ToMainLow_sendmsg(uc_ptr->buflen, MSGT_OVERRUN, (void *) uc_ptr->buffer);
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
        ToMainLow_sendmsg(0, MSGT_OVERRUN, (void *) 0);
    }
}

void init_uart_recv(uart_comm *uc) {
    uc_ptr = uc;
    uc_ptr->buflen = 0;
    payload_length = 0;
    checksum_recv_value = 0;
    checksum_calc_value = 0;
}

void uart_send_array(char* data, char length) {
    //TODO: Create logic to prevent you from overriding the current buffer if
    //it has not yet been sent. 
    uint8_t i;
    for(i = 0; i<length; i++) {
        uc_ptr->outBuff[i] = *(data + i);
    }
    uc_ptr->outLength = length;
    uc_ptr->outIndex = 1;
    WriteUSART(uc_ptr->outBuff[0]);
    PIR1bits.TXIF = 0;
    PIE1bits.TXIE = 1;
}
//Used to send multiple char. Will get called when uart_send_array is called
//Brian says DONT DELETE! I will find you.
void uart_send_int_handler() {
    uart_send(uc_ptr->outBuff[uc_ptr->outIndex++]);
    if (uc_ptr->outLength <= uc_ptr->outIndex)
    {
        // done sending message
        PIE1bits.TXIE = 0;
        PIR1bits.TXIF = 0;

    }
}

void uart_send(char data){
    //TODO possibly create logic to (without using a while) prevent writing if the buffer is not
    //clear
    WriteUSART(data);
//    debugNum(data);
}