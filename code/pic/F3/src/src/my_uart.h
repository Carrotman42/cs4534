#ifndef __my_uart_h
#define __my_uart_h

#include "messages.h"

#define MAXUARTBUF 15
#if (MAXUARTBUF > MSGLEN)
#define MAXUARTBUF MSGLEN
#endif
typedef struct __uart_comm {
    unsigned char buffer[MAXUARTBUF];
    unsigned char buflen;
    unsigned char outBuff[MAXUARTBUF];
    unsigned char outLength;
    unsigned char outIndex;
    unsigned char checksum;
    unsigned char status;
} uart_comm;

void uart_recv_wifly_debug_handler();
void init_uart_recv(uart_comm *);
void uart_recv_int_handler(void);
void uart_send(unsigned char);
void uart_send_int_handler();
void uart_send_array(char* data, char length);

//status codes
#define UART_IDLE 0
#define UART_TX 1
//end status codes

extern unsigned char wifly_setup;

#endif
