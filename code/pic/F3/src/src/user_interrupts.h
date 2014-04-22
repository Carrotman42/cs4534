#ifndef __user_interrupts
#define __user_interrupts
#include <stdbool.h>

// interrupts defined by the "user" and that are called from
// interrupts.c -- the "user" needs to insert these into
// interrupts.c because it, of course, doesn't know which
// interrupt handlers you would like to call

void timer0_int_handler(void);

void timer1_int_handler(void);

void timer2_int_handler(void);

#ifdef MASTER_PIC
void color_sensor_int_handler(void);
#endif

// include the handler from my uart code
#include "my_uart.h"

// include the i2c interrupt handler definitions
#include "my_i2c.h"
#include <stdint.h>

extern unsigned char datareq;


#ifdef MOTOR_PIC
void setM1Tick(uint16_t motor1);
void setM2Tick(uint16_t motor2);
void setCommandDone();
bool getCommandDone();
int getM1Ticks();
int getM2Ticks();
#endif

#endif
