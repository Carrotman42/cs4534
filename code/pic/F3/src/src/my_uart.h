#ifndef __my_uart_h
#define __my_uart_h

#include "messages.h"

#define MAXUARTBUF 10
#if (MAXUARTBUF > MSGLEN)
#define MAXUARTBUF MSGLEN
#endif
typedef struct __uart_comm {
    unsigned char buffer[MAXUARTBUF];
    unsigned char buflen;
    unsigned char outBuff[MAXUARTBUF];
    unsigned char outLength;
    unsigned char outIndex;
} uart_comm;

void init_uart_recv(uart_comm *);
void uart_recv_int_handler(void);
void uart_send(unsigned char);
void uart_send_int_handler();
void uart_send_array(char* data, char length);

#endif
