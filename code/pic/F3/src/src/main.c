#include "maindefs.h"
#include <stdio.h>
#ifndef __XC8
#include <usart.h>
#include <i2c.h>
#include <timers.h>
#else
#include <plib/usart.h>
#include <plib/i2c.h>
#include <plib/timers.h>
#endif
#include "interrupts.h"
#include "messages.h"
#include "my_uart.h"
#include "my_i2c.h"
//#include "uart_thread.h"
//#include "timer1_thread.h"
//#include "timer0_thread.h"
#include "debug.h"
#include "comm.h"
#include "motor.h"

#ifdef SENSOR_PIC
#include "my_adc.h"
#include "sensorcomm.h"
#include "my_ultrasonic.h"
#endif


//Setup configuration registers
#ifdef __USE18F45J10
// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer Enable bit (WDT disabled (control is placed on SWDTEN bit))
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable bit (Reset on stack overflow/underflow disabled)
#ifndef __XC8
// Have to turn this off because I don't see how to enable this in the checkboxes for XC8 in this IDE
#pragma config XINST = ON       // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode enabled)
#else
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode enabled)
#endif

// CONFIG1H
#pragma config CP0 = OFF        // Code Protection bit (Program memory is not code-protected)

// CONFIG2L
#pragma config FOSC = HSPLL     // Oscillator Selection bits (HS oscillator, PLL enabled and under software control)
#pragma config FOSC2 = ON       // Default/Reset System Clock Select bit (Clock selected by FOSC as system clock is enabled when OSCCON<1:0> = 00)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Two-Speed Start-up (Internal/External Oscillator Switchover) Control bit (Two-Speed Start-up enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = DEFAULT // CCP2 MUX bit (CCP2 is multiplexed with RC1)

#else

#ifdef __USE18F2680
#pragma config OSC = IRCIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config BBSIZ = 1024     // Boot Block Size Select bits (1K words (2K bytes) Boot Block)
#ifndef __XC8
// Have to turn this off because I don't see how to enable this in the checkboxes for XC8 in this IDE
#pragma config XINST = ON       // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode enabled)
#endif

#else

#ifdef __USE18F26J50

// PIC18F26J50 Configuration Bit Settings

// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config PLLDIV = 3       // PLL Prescaler Selection bits (Divide by 3 (12 MHz oscillator input))
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset  (Disabled)
#pragma config XINST = ON       // Extended Instruction Set (Enabled)

// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = HSPLL      // Oscillator (HS+PLL, USB-HS+PLL)
#pragma config T1DIG = OFF      // T1OSCEN Enforcement (Secondary Oscillator clock source may not be selected)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator (High-power operation)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L
#pragma config DSWDTOSC = T1OSCREF// DSWDT Clock Select (DSWDT uses T1OSC/T1CKI)
#pragma config RTCOSC = T1OSCREF// RTCC Clock Select (RTCC uses T1OSC/T1CKI)
#pragma config DSBOREN = OFF    // Deep Sleep BOR (Disabled)
#pragma config DSWDTEN = OFF    // Deep Sleep Watchdog Timer (Disabled)
#pragma config DSWDTPS = G2     // Deep Sleep Watchdog Postscaler (1:2,147,483,648 (25.7 days))

// CONFIG3H
#pragma config IOL1WAY = ON     // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set once)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_63   // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 63)
#pragma config WPEND = PAGE_WPFP// Write/Erase Protect Region Select (valid when WPDIS = 0) (Page WPFP<5:0> through Configuration Words erase/write protected)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<5:0>/WPEND region ignored)

#else

#ifdef __USE18F46J50

// PIC18F46J50 Configuration Bit Settings

// CONFIG1L
#pragma config WDTEN = OFF      // Watchdog Timer (Disabled - Controlled by SWDTEN bit)
#pragma config PLLDIV = 3       // PLL Prescaler Selection bits (Divide by 3 (12 MHz oscillator input))
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset (Disabled)
#ifndef __XC8
// Have to turn this off because I don't see how to enable this in the checkboxes for XC8 in this IDE
#pragma config XINST = ON       // Extended Instruction Set (Enabled)
#else
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)
#endif

// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = HSPLL      // Oscillator (HS+PLL, USB-HS+PLL)
#pragma config T1DIG = OFF      // T1OSCEN Enforcement (Secondary Oscillator clock source may not be selected)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator (High-power operation)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L
#pragma config DSWDTOSC = T1OSCREF// DSWDT Clock Select (DSWDT uses T1OSC/T1CKI)
#pragma config RTCOSC = T1OSCREF// RTCC Clock Select (RTCC uses T1OSC/T1CKI)
#pragma config DSBOREN = OFF    // Deep Sleep BOR (Disabled)
#pragma config DSWDTEN = OFF    // Deep Sleep Watchdog Timer (Disabled)
#pragma config DSWDTPS = G2     // Deep Sleep Watchdog Postscaler (1:2,147,483,648 (25.7 days))

// CONFIG3H
#pragma config IOL1WAY = ON     // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set once)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_63   // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 63)
#pragma config WPEND = PAGE_WPFP// Write/Erase Protect Region Select (valid when WPDIS = 0) (Page WPFP<5:0> through Configuration Words erase/write protected)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<5:0>/WPEND region ignored)

#else

Something is messed up.
The PIC selected is not supported or the preprocessor directives are wrong.

#endif
#endif
#endif
#endif



void main(void) {
    //char c;
    signed char length;
    unsigned char msgtype;
    unsigned char last_reg_recvd;
    uart_comm uc;
    i2c_comm ic;
    unsigned char msgbuffer[MSGLEN + 1];
    //    unsigned char to_send_buffer[MAX_I2C_SENSOR_DATA_LEN + HEADER_MEMBERS];
    //    uint8 to_send_len;
    //    int data_points_count = 0;
    //unsigned char i;
    //uart_thread_struct uthread_data; // info for uart_lthread
    //timer1_thread_struct t1thread_data; // info for timer1_lthread
    //timer0_thread_struct t0thread_data; // info for timer0_lthread

#ifdef __USE18F2680
    OSCCON = 0xFC; // see datasheet
    // We have enough room below the Max Freq to enable the PLL for this chip
    OSCTUNEbits.PLLEN = 1; // 4x the clock speed in the previous line
#else
#ifdef __USE18F45J10
    OSCCON = 0x82; // see datasheeet
    OSCTUNEbits.PLLEN = 0; // Makes the clock exceed the PIC's rated speed if the PLL is on
#else
#ifdef __USE18F26J50
    OSCCON = 0xE0; // see datasheeet
    OSCTUNEbits.PLLEN = 1;
#else
#ifdef __USE18F46J50
    OSCCON = 0xE0; //see datasheet
    OSCTUNEbits.PLLEN = 1;
#else
    Something is messed up.
            The PIC selected is not supported or the preprocessor directives are wrong.
#endif
#endif
#endif
#endif

            // initialize my uart recv handling code

            init_uart_recv(&uc);

    // initialize the i2c code
    init_i2c(&ic);

    // init the timer1 lthread
    //    init_timer1_lthread(&t1thread_data);

    // initialize message queues before enabling any interrupts
    init_queues();

#ifndef __USE18F26J50
    // set direction for PORTB to output
#ifndef MOTOR_PIC
    TRISB = 0x0;
    LATB = 0x0;
#endif

#endif

    // how to set up PORTA for input (for the V4 board with the PIC2680)
    /*
            PORTA = 0x0;	// clear the port
            LATA = 0x0;		// clear the output latch
            ADCON1 = 0x0F;	// turn off the A2D function on these pins
            // Only for 40-pin version of this chip CMCON = 0x07;	// turn the comparator off
            TRISA = 0x0F;	// set RA3-RA0 to inputs
     */

    // initialize Timers
#ifndef MASTER_PIC
#ifdef SENSOR_PIC
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_16);
#elif !defined(MOTOR_PIC)
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_4);
#else
    OpenTimer0(TIMER_INT_ON & T0_8BIT & T0_PS_1_1 & T0_SOURCE_EXT);
#endif
#else
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_32);
#endif

#ifdef __USE18F26J50
    // MTJ added second argument for OpenTimer1()
    OpenTimer1(TIMER_INT_ON & T1_SOURCE_FOSC_4 & T1_PS_1_8 & T1_16BIT_RW & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF, 0x0);
#else
#ifdef __USE18F46J50
    OpenTimer1(TIMER_INT_ON & T1_SOURCE_FOSC_4 & T1_PS_1_8 & T1_16BIT_RW & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF, 0x0);
#else
#ifndef MOTOR_PIC
    OpenTimer1(TIMER_INT_ON & T1_PS_1_8 & T1_16BIT_RW & T1_SOURCE_INT & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
#else
    OpenTimer1(TIMER_INT_ON & T1_PS_1_1 & T1_16BIT_RW & T1_SOURCE_EXT & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);
    WRITETIMER1(0xFFE8); // leave in here, encoder's for motor 2 doesn't work without it
#endif
#endif
#endif

    // Decide on the priority of the enabled peripheral interrupts
    // 0 is low, 1 is high
    // Timer1 interrupt
    IPR1bits.TMR1IP = 1;
    // USART RX interrupt
    IPR1bits.RCIP = 0;
    IPR1bits.TXIP = 0;
    // I2C interrupt
    IPR1bits.SSPIP = 1;

    //set i2c int high
    PIE1bits.SSPIE = 1;



#ifdef SENSOR_PIC
    //resetAccumulators();
    init_adc();
    initUS();

    // must specifically enable the I2C interrupts
    IPR1bits.ADIP = 0;
    
    // configure the hardware i2c device as a slave (0x9E -> 0x4F) or (0x9A -> 0x4D)
    i2c_configure_slave(SENSOR_ADDR << 1); //address 0x10
#elif defined MOTOR_PIC
    i2c_configure_slave(MOTOR_ADDR << 1); //address 0x20
#elif defined PICMAN
    i2c_configure_slave(PICMAN_ADDR << 1); //address 0x10,different bus from sensor
#elif defined I2C_MASTER
    //sending clock frequency
    i2c_configure_master(); //12MHz clock set hardcoded
#endif


    // configure the hardware USART device
#ifdef __USE18F26J50
    Open1USART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
            USART_CONT_RX & USART_BRGH_LOW, 0x19);
#else
#ifdef __USE18F46J50

#ifndef MOTOR_PIC
    Open1USART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
            USART_CONT_RX & USART_BRGH_LOW, 38);
#else
    Open1USART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
            USART_CONT_RX & USART_BRGH_LOW, 0x19);
#endif

#else
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
            USART_CONT_RX & USART_BRGH_HIGH, 38);
    //    BAUDCONbits.BRG16 = 1;
    //    TXSTAbits.TXEN = 1;
    //    RCSTAbits.SPEN = 1;
    //    RCSTAbits.CREN = 1;
#endif
#endif

    // Peripheral interrupts can have their priority set to high or low
    // enable high-priority interrupts and low-priority interrupts
    enable_interrupts();
    LATBbits.LB7 = 0;
    WRITETIMER0(0x00FF);

    // loop forever
    // This loop is responsible for "handing off" messages to the subroutines
    // that should get them.  Although the subroutines are not threads, but
    // they can be equated with the tasks in your task diagram if you
    // structure them properly.

    //TODO: delete the test line
    //    calcRevMotor1(20);
    //    calcRevMotor2(20);
    //    forward();

    //    turnRight90_onSpot();
    
//    doEverything(2, 1);    // turn left
    //calcRevMotor1(10);
    //calcRevMotor2(10);
//    forward(0);
//    debugNum(1);
    //forward2Rev();
    //turnLeft();
    //forward2Rev();


//        calcRevMotor2(10);
//        calcRevMotor1(10);
//        forward2(1);
 //   reverse(7);
//    turnLeft90_onSpot();
//        reverse(5);
//        turnLeft90_onSpot();

//        forward(0);
        //for (int i = 0; i < 50; i++);
        //setKill();
//        reverse(4);
//        funFunc(0);
    //    funFunc();

     //   readjustRight();

    //    calcRevMotor1(1);
    //    turnLeft90_onSpot();
    //    turnRight90_onSpot();
    while (1) {
        // Call a routine that blocks until either on the incoming
        // messages queues has a message (this may put the processor into
        // an idle mode)
        block_on_To_msgqueues();

        //We have a bunch of queues now - ToMainHigh, ToMainLow, FromMainHigh, FromMainLow,
        //FromUARTInt, and FromI2CInt
        //From queues are most important because they will be called repeatedly with busy info
        //Int queues are second because we'll often get data from either UART or I2C
        //ToMain are least

        length = FromMainHigh_recvmsg(MSGLEN, &msgtype, (void *) msgbuffer);
        if (length < 0) {
            // no message, check the error code to see if it is concern
            if (length != MSGQUEUE_EMPTY) {
                // This case be handled by your code.
            }
        } else {
            switch (msgtype) {
#ifdef I2C_MASTER
                case MSGT_MASTER_RECV_BUSY:
                {
                    //retry
                    debugNum(4);
                    i2c_master_recv(msgbuffer[0]);
                    break;
                };
                case MSGT_MASTER_SEND_BUSY:
                {
                    //retry
                    debugNum(8);
                    i2c_master_send(msgbuffer[0], length - 1, msgbuffer + 1); // point to second position (actual msg start)
                    break;
                };
                case MSGT_MASTER_SEND_NO_RAW_BUSY:
                {
                    i2c_master_send_no_raw(msgbuffer[0], length-1, msgbuffer + 1);
                };
#endif
                case MSGT_UART_TX_BUSY:
                {
                    // TODO: take out for now
                    //uart_send_array(msgbuffer, length);
                    break;
                };
                default:
                    break;
            }
        }

        length = FromUARTInt_recvmsg(MSGLEN, &msgtype, (void *) msgbuffer);
        if (length < 0) {
            // no message, check the error code to see if it is concern
            if (length != MSGQUEUE_EMPTY) {
                // This case be handled by your code.
            }
        } else {
            switch (msgtype) {
                case MSGT_OVERRUN:
                    break;
                case MSGT_UART_DATA:
                {
#ifdef PICMAN
                    setRoverDataLP(msgbuffer);
                    handleRoverDataLP();
#elif defined(MASTER_PIC) || defined(ROVER_EMU)
                    setBrainDataLP(msgbuffer); //pass data received and tell will pass over i2c
                    handleMessageLP(UART_COMM, I2C_COMM); //sends the response and then sets up the command handling
#endif
                    break;
                };
                case MSGT_UART_RECV_FAILED:
                {
                    debugNum(1);
                    debugNum(2);
                    debugNum(1);
                    debugNum(2);
                    break;
                };
                case MSGT_UART_TX_BUSY:
                {
                    // TODO: take out for now
                    //uart_send_array(msgbuffer, length);
                    break;
                };
                default:
                    break;
            }
        }

        length = FromI2CInt_recvmsg(MSGLEN, &msgtype, (void *) msgbuffer);
        if (length < 0) {
            // no message, check the error code to see if it is concern
            if (length != MSGQUEUE_EMPTY) {
                // This case be handled by your code.
            }
        } else {
            switch (msgtype) {
                case MSGT_I2C_DATA:
                {
#if defined(MASTER_PIC) || defined(ARM_EMU)
                    //handle whatever data will come through via i2c
                    //msgbuffer can hold real data - error codes will be returned through the error cases
                    setRoverDataLP(msgbuffer);
                    handleRoverDataLP();
#else
                    setBrainDataLP(msgbuffer);
#endif
                    break;
                };
                case MSGT_I2C_RQST:
                {
#if defined(MOTOR_PIC) || defined(SENSOR_PIC)
                    handleMessageLP(I2C_COMM, I2C_COMM);
#elif defined(PICMAN)
                    handleMessageLP(I2C_COMM, UART_COMM);
#endif
                    break;
                };
                case MSGT_I2C_DBG:
                {
                    // Here is where you could handle debugging, if you wanted
                    // keep track of the first byte received for later use (if desired)
                    last_reg_recvd = msgbuffer[0];
                    break;
                };
#ifdef MASTER_PIC
                case MSGT_I2C_MASTER_RECV_FAILED:
                {
                    uart_send_array(msgbuffer, length);
                    break;
                };
                case MSGT_I2C_MASTER_SEND_FAILED:
                {
                    uart_send_array(msgbuffer, length);
                    break;
                };
                case MSGT_MASTER_RECV_BUSY:
                {
                    //retry
                    debugNum(4);
                    i2c_master_recv(msgbuffer[0]);
                    break;
                };
                case MSGT_MASTER_SEND_BUSY:
                {
                    //retry
                    debugNum(8);
                    i2c_master_send(msgbuffer[0], length - 1, msgbuffer + 1); // point to second position (actual msg start)
                    break;
                };
#endif
                default:
                    break;
            }
        }





        length = ToMainHigh_recvmsg(MSGLEN, &msgtype, (void *) msgbuffer);
        if (length < 0) {
            // no message, check the error code to see if it is concern
            if (length != MSGQUEUE_EMPTY) {
                // This case be handled by your code.
            }
        } else {
            switch (msgtype) {
                case MSGT_TIMER0:
                {
                    //timer0_lthread(&t0thread_data, msgtype, length, msgbuffer);
                    break;
                };
                    //                #ifdef I2C_MASTER
                    //                case MSGT_MASTER_RECV_BUSY:
                    //                {
                    //                    //retry
                    //                    debugNum(4);
                    //                    i2c_master_recv(msgbuffer[0]);
                    //                    break;
                    //                };
                    //                case MSGT_MASTER_SEND_BUSY:
                    //                {
                    //                    //retry
                    //                    debugNum(8);
                    //                    i2c_master_send(msgbuffer[0], length-1, msgbuffer + 1); // point to second position (actual msg start)
                    //                    break;
                    //                };
                    //                #endif
                    //                case MSGT_I2C_DATA:
                    //                {
                    //                    debugNum(4);
                    //#if defined(MASTER_PIC) || defined(ARM_EMU)
                    //                    //handle whatever data will come through via i2c
                    //                    //msgbuffer can hold real data - error codes will be returned through the error cases
                    //                    setRoverDataLP(msgbuffer);
                    //                    handleRoverDataLP();
                    //#else
                    //                    setBrainDataLP(msgbuffer);
                    //#endif
                    //                    debugNum(4);
                    //                    break;
                    //                };
                    //                case MSGT_I2C_RQST:
                    //                {
                    //#if defined(MOTOR_PIC) || defined(SENSOR_PIC)
                    //                    handleMessageLP(I2C_COMM, I2C_COMM);
                    //#elif defined(PICMAN)
                    //                    handleMessageLP(I2C_COMM, UART_COMM);
                    //#endif
                    //                    break;
                    //                };
                    //                case MSGT_I2C_DBG:
                    //                {
                    //                    // Here is where you could handle debugging, if you wanted
                    //                    // keep track of the first byte received for later use (if desired)
                    //                    last_reg_recvd = msgbuffer[0];
                    //                    break;
                    //                };
                    //#ifdef MASTER_PIC
                    //                case MSGT_I2C_MASTER_RECV_FAILED:
                    //                {
                    //                    uart_send_array(msgbuffer, length);
                    //                    break;
                    //                };
                    //                case MSGT_I2C_MASTER_SEND_FAILED:
                    //                {
                    //                    uart_send_array(msgbuffer, length);
                    //                    break;
                    //                };
                    //#endif
                case MSGT_AD:
                {
#ifdef SENSOR_PIC
                    //addDataPoints(sensorADid, msgbuffer, length);
#endif
                    break;
                };

                default:
                {
                    // Your code should handle this error
                    break;
                };
            };
        }

        // Check the low priority queue
        length = ToMainLow_recvmsg(MSGLEN, &msgtype, (void *) msgbuffer);
        if (length < 0) {
            // no message, check the error code to see if it is concern
            if (length != MSGQUEUE_EMPTY) {
                // Your code should handle this situation
            }
        } else {
            switch (msgtype) {
                case MSGT_AD:
                {
#ifdef SENSOR_PIC
                    //addDataPoints(sensorADid, msgbuffer, length);
#endif
                    break;
                };
                case MSGT_TIMER1:
                {
                    //timer1_lthread(&t1thread_data, msgtype, length, msgbuffer);
                    break;
                };
                    //                case MSGT_OVERRUN:
                    //                    break;
                    //                case MSGT_UART_DATA:
                    //                {
                    //#ifdef PICMAN
                    //                    setRoverDataLP(msgbuffer);
                    //                    handleRoverDataLP();
                    //#elif defined(MASTER_PIC) || defined(ROVER_EMU)
                    //                    setBrainDataLP(msgbuffer);//pass data received and tell will pass over i2c
                    //                    handleMessageLP(UART_COMM, I2C_COMM); //sends the response and then sets up the command handling
                    //#endif
                    //                    break;
                    //                };
                    //                case MSGT_UART_RECV_FAILED:
                    //                {
                    //                    debugNum(1);
                    //                    debugNum(2);
                    //                    debugNum(1);
                    //                    debugNum(2);
                    //                    break;
                    //                };
                default:
                {
                    // Your code should handle this error
                    break;
                };
            };
        }
    }
//
}