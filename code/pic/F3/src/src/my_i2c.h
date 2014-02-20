#ifndef __my_i2c_h
#define __my_i2c_h

#include "messages.h"
#include "maindefs.h"
#include "../../../../common/communication/brain_rover.h"

#define MAXI2CBUF MSGLEN
#define MAX_I2C_SENSOR_DATA_LEN 100

#ifdef I2C_MASTER
typedef struct __i2c_comm {
    unsigned char buffer[MAXI2CBUF];
    unsigned char buflen;
    unsigned char bufind;
    unsigned char event_count;
    unsigned char status;
    unsigned char error_code;
    unsigned char error_count;
    unsigned char outbuffer[MAX_I2C_SENSOR_DATA_LEN + 3]; // +3 for overhead
    unsigned char outbuflen;
    unsigned char outbufind;
    unsigned char baud_rate;
    unsigned char txnrx; //1 if tx, 0 if rx
} i2c_comm;

void i2c_configure_master();
unsigned char i2c_master_send(unsigned char, unsigned char,unsigned char *);
unsigned char i2c_master_recv(unsigned char);
unsigned char load_i2c_data();
void handle_repeat_start();
uint8 check_if_send_stop();
void send_stop();
uint8 receive_data();

void i2c_tx_handler();
void i2c_rx_handler();

#define I2C_MASTER_SEND 0x8
#define I2C_STOPPED 0x9
#define I2C_ACK 0xa


#else //!I2C_MASTER = I2C_SLAVE
typedef struct __i2c_comm {
    unsigned char buffer[MAXI2CBUF];
    unsigned char buflen;
    unsigned char event_count;
    unsigned char status;
    unsigned char error_code;
    unsigned char error_count;
    unsigned char outbuffer[MAX_I2C_SENSOR_DATA_LEN + ROVERMSG_MEMBERS]; // +3 for overhead
    unsigned char outbuflen;
    unsigned char outbufind;
    unsigned char slave_addr;
} i2c_comm;


void start_i2c_slave_reply(unsigned char,unsigned char *);
void i2c_configure_slave(unsigned char);

#define I2C_SLAVE_SEND 0x8
#endif //I2C_MASTER


#define I2C_IDLE 0x5
#define I2C_STARTED 0x6
#define	I2C_RCV_DATA 0x7


#define I2C_ERR_THRESHOLD 1
#define I2C_ERR_OVERRUN 0x4
#define I2C_ERR_NOADDR 0x5
#define I2C_ERR_NODATA 0x6
#define I2C_ERR_MSGTOOLONG 0x7
#define I2C_ERR_MSG_TRUNC 0x8

void init_i2c(i2c_comm *);
void i2c_int_handler(void);

#endif