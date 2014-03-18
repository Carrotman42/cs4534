#include "motor.h"


// both motors in reverse
void reverse()
{
    unsigned char test[2] = {0x05, 0x83};
    uart_send_array(&test, 2);
}

// both motors in forward
void forward()
{
    unsigned char test[2] = {0x70, 0xFF};
    uart_send_array(&test, 2);
}

// motor 1 forwards, motor 2 stops
void forwardMotor1()
{
    // unsigned char test[2] = {0x6A, 0xC0};
    unsigned char test[2] = {0x50 , 0xC0};
    uart_send_array(&test, 2);
}

// motor 1 reverse, motor 2 stops
void reverseMotor1()
{
    unsigned char test[2] = {0x21, 0xC0};
    uart_send_array(&test, 2);
}

// motor 2 forwards, motor 1 stops
void forwardMotor2()
{
    unsigned char test[2] = {0x40, 0xC5};     // REALLY SLOW
    uart_send_array(&test, 2);
}

// motor 2 reverse, motor 1 stops
void reverseMotor2()
{
    unsigned char test[2] = {0x40, 0x85};
    uart_send_array(&test, 2);
}

// stops both motors
void stop()
{
    // 0x00 stops both wheels
    unsigned char test[2] = {0x40, 0xC0};
    uart_send_array(&test, 2);
}

// int handler for the encoders for motor 0
// 6127
void motor0_int_handler()
{
    motor1Ticks++;
}

// int handler for the encoders for motor 1
void motor1_int_handler()
{
    motor2Ticks++;
}

void resetTicks()
{
    motor1Ticks = 0;
    motor2Ticks = 0;
}