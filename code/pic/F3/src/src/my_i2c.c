#include "maindefs.h"
#ifndef __XC8
#include <i2c.h>
#else
#include <plib/i2c.h>
#endif
#include "my_i2c.h"
#include "debug.h"
#include "my_uart.h"

#ifdef DEBUG_ON
#include "testAD.h"
#endif

#ifdef SENSOR_PIC
#include "sensorcomm.h"
#endif


static i2c_comm *ic_ptr;

// set up the data structures for this i2c code
// should be called once before any i2c routines are called
void init_i2c(i2c_comm *ic) {
    ic_ptr = ic;
    ic_ptr->buflen = 0;
    ic_ptr->event_count = 0;
    ic_ptr->status = I2C_IDLE;
    ic_ptr->error_count = 0;
}


#ifdef I2C_MASTER
// Configure for I2C Master mode -- the variable "slave_addr" should be stored in
//   i2c_comm (as pointed to by ic_ptr) for later use.
//SSPADD=(FOSC / (4 * BAUD)) - 1;
void i2c_configure_master() {
    SSPCON1bits.SSPM = 0x8; // Master with Baud rate as set below
    SSPCON1bits.SSPEN = 0x1; //Enable SDA/SCL
    SSPSTATbits.SMP = 0x1;
    SSPADD = (12000000 / (4*100000))-1; //should be 0x1D for a 12 MHz clock
}

// Sending in I2C Master mode [slave write]
// 		returns -1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the sending of an i2c message -- interrupt handler will take care of
//   completing the message send.  When the i2c message is sent (or the send has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_SEND_COMPLETE if
//   the send was successful and an internal_message of type MSGT_MASTER_SEND_FAILED if the
//   send failed (e.g., if the slave did not acknowledge).  Both of these internal_messages
//   will have a length of 0.
// The subroutine must copy the msg to be sent from the "msg" parameter below into
//   the structure to which ic_ptr points [there is already a suitable buffer there].

//addr is the actual address.  It will be shifted here
unsigned char i2c_master_send(unsigned char addr, unsigned char length, unsigned char *msg) {
    if(ic_ptr->status != I2C_IDLE){
        //copy addr and msg into a single array
        char tempbuf[MAX_I2C_SENSOR_DATA_LEN +1];
        tempbuf[0] = addr;
        int i = 1;
        for(i; i < length+1; i++){
            tempbuf[i] = msg[i-1];
        }
        ToMainHigh_sendmsg(length+1, MSGT_MASTER_RECV_BUSY, tempbuf);
        //debugNum(2);
        return -1;
    }
    ic_ptr->txnrx = 1;
    char buf_addr = (addr << 1) & 0xFE; // explicitely make sure that the lsb is 0 and et the addr in top 7 bits
    ic_ptr->outbuffer[0] = buf_addr;
    int i =1;
    for(i; i < length + 1; i++){
        ic_ptr->outbuffer[i] = msg[i-1];
    }
    ic_ptr->outbuflen = length + 1; //char length + addr byte
    ic_ptr->outbufind = 0; //start at 0th pos.  addr will be written in after S int happens
    SSPCON2bits.SEN = 1; //send start signal
    ic_ptr->status = I2C_STARTED;

    return(0);
}

// Receiving in I2C Master mode [slave read]
// 		returns -1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the receiving of an i2c message -- interrupt handler will take care of
//   completing the i2c message receive.  When the receive is complete (or has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_RECV_COMPLETE if
//   the receive was successful and an internal_message of type MSGT_MASTER_RECV_FAILED if the
//   receive failed (e.g., if the slave did not acknowledge).  In the failure case
//   the internal_message will be of length 0.  In the successful case, the
//   internal_message will contain the message that was received [where the length
//   is determined by the parameter passed to i2c_master_recv()].
// The interrupt handler will be responsible for copying the message received into

unsigned char i2c_master_recv(unsigned char addr) {
    if(ic_ptr->status != I2C_IDLE){
        ToMainHigh_sendmsg(1, MSGT_MASTER_RECV_BUSY, &addr);
        //debugNum(2);
        return -1;
    }
    ic_ptr->txnrx = 0;
    char buf_addr = (addr << 1) | 0x1; // set addr in top 7 bits and set lsb
    ic_ptr->outbuffer[0] = buf_addr;
    ic_ptr->outbuflen = 1; //just enough room for addr
    ic_ptr->outbufind = 0; //set for addr
    ic_ptr->buflen = 3; //reset the buffer, we'll at LEAST read 3 bytes
    ic_ptr->bufind = 0;
    SSPCON2bits.SEN = 1;
    ic_ptr->status = I2C_STARTED;
    //debugNum(1);
    return(0);
}

//load the i2c data into the buffer.
unsigned char load_i2c_data(){
    if(ic_ptr->status == I2C_STARTED) ic_ptr->status = I2C_MASTER_SEND; //change the status to "sending data" on the next interrupt
    SSPBUF = ic_ptr->outbuffer[ic_ptr->outbufind++];
    return SSPCON1bits.WCOL;
}

void handle_repeat_start(){
    SSPCON2bits.RSEN = 1;
}

uint8 check_if_send_stop(){
    if(ic_ptr->outbufind == ic_ptr->outbuflen){
        return 1;
    }
    return 0;
}

void send_stop(){
    ic_ptr->status = I2C_STOPPED;
    SSPCON2bits.PEN = 1;
}

void i2c_tx_handler(){
    switch(ic_ptr->status){
        case(I2C_STARTED):
            load_i2c_data(); //start handled same way as sending data - address should already be loaded.
            break;
        case(I2C_MASTER_SEND):
            if((SSPCON2bits.ACKSTAT == 1) || check_if_send_stop()){ //ack not received or it should stop naturally
                //ic_ptr->outbufind--; //Resend last byte
                send_stop();
            }
            else{
                if(load_i2c_data() == 1) //WCOL bit set
                    send_stop();
            }
            break;
        case(I2C_STOPPED): //stop
            ic_ptr->status = I2C_IDLE;
            break;
        default:
            break;
    }
}

//return 1 when we want to stop the transfer (on error or all wanted bytes are read), 0 otherwise
uint8 receive_data(){
    if(!SSPSTATbits.BF){//nothing in buffer
        SSPCON2bits.ACKDT = 1;
        SSPCON2bits.ACKEN = 1;
        return 1;
    }
    unsigned char recv = SSPBUF;
    ic_ptr->buffer[ic_ptr->bufind] = recv;
    if(++ic_ptr->bufind == 3){
        ic_ptr->buflen = recv+3; //3rd byte is the payload length, add the 3 bytes already received to the buffer length
    }

    if(ic_ptr->bufind >= ic_ptr->buflen){ //at end of bytes that slave told us to read
        SSPCON2bits.ACKDT = 1;
        SSPCON2bits.ACKEN = 1;
        return 1;
    }

    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    ic_ptr->status = I2C_ACK;
    return 0;
}

void i2c_rx_handler(){
    switch(ic_ptr->status){
        case(I2C_STARTED):
            load_i2c_data();
            // //need to set this after transmitting
            break;
        case(I2C_MASTER_SEND): //sent the addr
            if(SSPCON2bits.ACKSTAT == 1){ //ack not received
                send_stop();
            }
            else{
                ic_ptr->status = I2C_RCV_DATA;
                SSPCON2bits.RCEN = 1;
            }
            break;
        case(I2C_RCV_DATA):
            if(receive_data() == 1){ //receive is finished
                ic_ptr->status = I2C_NACK;
            }
            break;
        case(I2C_ACK):
            ic_ptr->status = I2C_RCV_DATA;
            SSPCON2bits.RCEN = 1;
            break;
        case(I2C_NACK):
            send_stop();
            break;
        case(I2C_STOPPED):
            ic_ptr->status = I2C_IDLE;
            break;
        default:
            break;
    }
}


void i2c_int_handler(){
    if(ic_ptr->txnrx)
        i2c_tx_handler();
    else
        i2c_rx_handler();
}


#else
void start_i2c_slave_reply(unsigned char length, unsigned char *msg) {

    for (ic_ptr->outbuflen = 0; ic_ptr->outbuflen < length; ic_ptr->outbuflen++) {
        ic_ptr->outbuffer[ic_ptr->outbuflen] = msg[ic_ptr->outbuflen];
    }
    ic_ptr->outbuflen = length;
    ic_ptr->outbufind = 1; // point to the second byte to be sent

    // put the first byte into the I2C peripheral
    SSPBUF = ic_ptr->outbuffer[0];
    // we must be ready to go at this point, because we'll be releasing the I2C
    // peripheral which will soon trigger an interrupt
    SSPCON1bits.CKP = 1;

}

// an internal subroutine used in the slave version of the i2c_int_handler

void handle_start(unsigned char data_read) {
    ic_ptr->event_count = 1;
    ic_ptr->buflen = 0;
    // check to see if we also got the address
    if (data_read) {
        if (SSPSTATbits.D_A == 1) {
            // this is bad because we got data and
            // we wanted an address
            ic_ptr->status = I2C_IDLE;
            ic_ptr->error_count++;
            ic_ptr->error_code = I2C_ERR_NOADDR;
        } else {
            if (SSPSTATbits.R_W == 1) {
                ic_ptr->status = I2C_SLAVE_SEND;
            } else {
                ic_ptr->status = I2C_RCV_DATA;
            }
        }
    } else {
        ic_ptr->status = I2C_STARTED;
    }
}

// this is the interrupt handler for i2c -- it is currently built for slave mode
// -- to add master mode, you should determine (at the top of the interrupt handler)
//    which mode you are in and call the appropriate subroutine.  The existing code
//    below should be moved into its own "i2c_slave_handler()" routine and the new
//    master code should be in a subroutine called "i2c_master_handler()"

void i2c_int_handler() {

    unsigned char i2c_data;
    unsigned char data_read = 0;
    unsigned char data_written = 0;
    unsigned char msg_ready = 0;
    unsigned char msg_to_send = 0;
    unsigned char overrun_error = 0;
    unsigned char error_buf[3];

    // clear SSPOV
    if (SSPCON1bits.SSPOV == 1) {
        SSPCON1bits.SSPOV = 0;
        // we failed to read the buffer in time, so we know we
        // can't properly receive this message, just put us in the
        // a state where we are looking for a new message
        ic_ptr->status = I2C_IDLE;
        overrun_error = 1;
        ic_ptr->error_count++;
        ic_ptr->error_code = I2C_ERR_OVERRUN;
    }
    // read something if it is there
    if (SSPSTATbits.BF == 1) {
        i2c_data = SSPBUF;
        data_read = 1;
    }

    if (!overrun_error) {
        switch (ic_ptr->status) {
            case I2C_IDLE:
            {
                // ignore anything except a start
                if (SSPSTATbits.S == 1) {
                    handle_start(data_read);
                    // if we see a slave read, then we need to handle it here
                    if (ic_ptr->status == I2C_SLAVE_SEND) {
                        data_read = 0;
                        msg_to_send = 1;
                    }
                }
                break;
            }
            case I2C_STARTED:
            {
                // in this case, we expect either an address or a stop bit
                if (SSPSTATbits.P == 1) {
                    // we need to check to see if we also read an
                    // address (a message of length 0)
                    ic_ptr->event_count++;
                    if (data_read) {
                        if (SSPSTATbits.D_A == 0) {
                            msg_ready = 1;
                        } else {
                            ic_ptr->error_count++;
                            ic_ptr->error_code = I2C_ERR_NODATA;
                        }
                    }
                    ic_ptr->status = I2C_IDLE;
                } else if (data_read) {
                    ic_ptr->event_count++;
                    if (SSPSTATbits.D_A == 0) {
                        if (SSPSTATbits.R_W == 0) { // slave write
                            ic_ptr->status = I2C_RCV_DATA;
                        } else { // slave read
                            ic_ptr->status = I2C_SLAVE_SEND;
                            msg_to_send = 1;
                            // don't let the clock stretching bit be let go
                            data_read = 0;
                        }
                    } else {
                        ic_ptr->error_count++;
                        ic_ptr->status = I2C_IDLE;
                        ic_ptr->error_code = I2C_ERR_NODATA;
                    }
                }
                break;
            }
            case I2C_SLAVE_SEND:
            {
                if (ic_ptr->outbufind < ic_ptr->outbuflen) {
//            setDBG(DBG2);
//            resetDBG(DBG2);
                    SSPBUF = ic_ptr->outbuffer[ic_ptr->outbufind];
                    ic_ptr->outbufind++;
                    data_written = 1;
                } else {
                    // we have nothing left to send
                    ic_ptr->status = I2C_IDLE;
                }
                break;
            }
            case I2C_RCV_DATA:
            {
                // we expect either data or a stop bit or a (if a restart, an addr)
                if (SSPSTATbits.P == 1) {
                    // we need to check to see if we also read data
                    ic_ptr->event_count++;
                    if (data_read) {
                        if (SSPSTATbits.D_A == 1) {
                            ic_ptr->buffer[ic_ptr->buflen] = i2c_data;
                            ic_ptr->buflen++;
                            msg_ready = 1;
                        } else {
                            ic_ptr->error_count++;
                            ic_ptr->error_code = I2C_ERR_NODATA;
                            ic_ptr->status = I2C_IDLE;
                        }
                    } else {
                        msg_ready = 1;
                    }
                    ic_ptr->status = I2C_IDLE;
                } else if (data_read) {
                    ic_ptr->event_count++;
                    if (SSPSTATbits.D_A == 1) {
                        ic_ptr->buffer[ic_ptr->buflen] = i2c_data;
                        ic_ptr->buflen++;
                    } else /* a restart */ {
                        if (SSPSTATbits.R_W == 1) {
                            ic_ptr->status = I2C_SLAVE_SEND;
                            msg_ready = 1;
                            msg_to_send = 1;
                            // don't let the clock stretching bit be let go
                            data_read = 0;
                        } else { /* bad to recv an address again, we aren't ready */
                            ic_ptr->error_count++;
                            ic_ptr->error_code = I2C_ERR_NODATA;
                            ic_ptr->status = I2C_IDLE;
                        }
                    }
                }
                break;
            }
        }
    }

    // release the clock stretching bit (if we should)
    if (data_read || data_written) {
        // release the clock

            //setDBG(DBG5);
            //resetDBG(DBG5);
        if (SSPCON1bits.CKP == 0) {
            SSPCON1bits.CKP = 1;
        }
    }

    // must check if the message is too long, if
    if ((ic_ptr->buflen > MAXI2CBUF - 2) && (!msg_ready)) {
        ic_ptr->status = I2C_IDLE;
        ic_ptr->error_count++;
        ic_ptr->error_code = I2C_ERR_MSGTOOLONG;
    }

    if (msg_ready) {

        #ifdef SENSOR_PIC
        ic_ptr->buffer[ic_ptr->buflen] = ic_ptr->event_count;
        setBrainReqData(ic_ptr->buffer);
        ////ToMainHigh_sendmsg(ic_ptr->buflen + 1, MSGT_I2C_DATA, (void *) ic_ptr->buffer);
        #elif defined(PICMAN) || defined(MOTOR_PIC)
        //ic_ptr->buffer[ic_ptr->buflen] = ic_ptr->event_count;
        //debugNum(1);
        if(is_high_priority()){ //would pass ic_ptr->buffer but it's global so no need
            uint8 i = 0;
            for(i; i < ic_ptr->buflen; i++){
                uart_send(ic_ptr->buffer[i]);
                uart_send(ic_ptr->buffer[i]);
            }
        }
        else{
            ToMainHigh_sendmsg(ic_ptr->buflen + 1, MSGT_I2C_DATA, (void *) ic_ptr->buffer);
        }
        #endif
        ic_ptr->buflen = 0;
    } else if (ic_ptr->error_count >= I2C_ERR_THRESHOLD) {
        error_buf[0] = ic_ptr->error_count;
        error_buf[1] = ic_ptr->error_code;
        error_buf[2] = ic_ptr->event_count;
        ToMainHigh_sendmsg(sizeof (unsigned char) *3, MSGT_I2C_DBG, (void *) error_buf);
        ic_ptr->error_count = 0;
    }
    if (msg_to_send) {
        debugNum(2);
        char outbuff[4];
        outbuff[0] = 0xa0;
        outbuff[1] = 0x66;
        outbuff[2] = 0x01;
        outbuff[3] = 0xaa;
        start_i2c_slave_reply(4, outbuff);
        //sendRequestedData();
        msg_to_send = 0;
    }
}

// setup the PIC to operate as a slave
// the address must include the R/W bit

void i2c_configure_slave(unsigned char addr) {

    // ensure the two lines are set for input (we are a slave)
#ifdef __USE18F26J50
    //THIS CODE LOOKS WRONG, SHOULDN'T IT BE USING THE TRIS BITS???
    PORTBbits.SCL1 = 1;
    PORTBbits.SDA1 = 1;
#else
#ifdef __USE18F46J50
    TRISBbits.TRISB4 = 1; //RB4 = SCL1
    TRISBbits.TRISB5 = 1; //RB5 = SDA1
#else
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
#endif
#endif

    // set the address
    SSPADD = addr;
    //OpenI2C(SLAVE_7,SLEW_OFF); // replaced w/ code below
    SSPSTAT = 0x0;
    SSPCON1 = 0x0;
    SSPCON2 = 0x0;
    SSPCON1 |= 0x0E; // enable Slave 7-bit w/ start/stop interrupts
    SSPSTAT |= SLEW_OFF;

#ifdef I2C_V3
    I2C1_SCL = 1;
    I2C1_SDA = 1;
#else 
#ifdef I2C_V1
    I2C_SCL = 1;
    I2C_SDA = 1;
#else
#ifdef __USE18F26J50
    PORTBbits.SCL1 = 1;
    PORTBbits.SDA1 = 1;
#else
#ifdef __USE18F46J50
    PORTBbits.SCL1 = 1;
    PORTBbits.SDA1 = 1;
#else
    __dummyXY=35;// Something is messed up with the #ifdefs; this line is designed to invoke a compiler error
#endif
#endif
#endif
#endif
    
    // enable clock-stretching
    SSPCON2bits.SEN = 1;
    SSPCON1 |= SSPENB;
    // end of i2c configure
}

#if defined(PICMAN) || defined(MOTOR_PIC)
//return 1 if high priority,0 otherwise
uint8 is_high_priority(){
    if(ic_ptr->buffer[0] == 0x01){
        return 1;
    }
    return 0;
}
#endif


#endif //I2C_MASTER