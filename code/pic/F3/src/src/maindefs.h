#ifndef __maindefs
#define __maindefs

#ifdef __XC8
#include <xc.h>
#ifdef _18F45J10
#define __USE18F45J10 1
#else
#ifdef _18F2680
#define __USE18F2680 1
#else
#ifdef _18F26J50
#define __USE18F26J50 1
#else
#ifdef _18F46J50
#define __USE18F46J50 1
#endif
#endif
#endif
#endif
#else
#ifdef __18F45J10
#define __USE18F45J10 1
#else
#ifdef __18F2680
#define __USE18F2680 1
#else
#ifdef __18F26J50
#define __USE18F26J50 1
#else
#ifdef __18F46J50
#define __USE18F46J50 1
#endif
#endif
#endif
#endif
#include <p18cxxx.h>
#endif

// Message type definitions

#define GLEN_DEBUG 2        // master pic = 1, slave pic = 0

#define MSGT_AD 50

#define MSGT_TIMER0 10
#define MSGT_TIMER1 11
#define MSGT_MAIN1 20
#define	MSGT_OVERRUN 30
#define MSGT_UART_DATA 31
#define MSGT_UART_RECV_FAILED 32
#define MSGT_UART_RX_BUSY 33
#define MSGT_UART_TX_BUSY 34
#define MSGT_I2C_DBG 41
#define	MSGT_I2C_DATA 40
#define MSGT_I2C_RQST 42
#define MSGT_I2C_MASTER_SEND_COMPLETE 43
#define MSGT_I2C_MASTER_SEND_FAILED 44
#define MSGT_I2C_MASTER_RECV_COMPLETE 45
#define MSGT_I2C_MASTER_RECV_FAILED 46
#define MSGT_MASTER_RECV_BUSY 47
#define MSGT_MASTER_SEND_BUSY 48
#define MSGT_COMM_BRAIN_BUSY 50
#define MSGT_COMM_ROVER_BUSY 51



#define DEBUG_ON

//#define ARM_EMU //arm emulator (really simple, just sends commands on a timer)
//#define ROVER_EMU //rover emulator (really simple, just sends dummy values back over uart and sends back frame data on a timer)

//#define PICMAN
//#define SENSOR_PIC
#define MOTOR_PIC
//#define MASTER_PIC


#if defined(MASTER_PIC) || defined(ARM_EMU)
#define I2C_MASTER
#else
#define I2C_SLAVE
#endif

#define MOTOR_ADDR 0x20
#define SENSOR_ADDR 0x10
#define PICMAN_ADDR 0x10


#include "my_uart.h"
#include "debug.h"
#include "my_i2c.h"

#endif

