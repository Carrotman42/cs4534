#include "maindefs.h"
#include "debug.h"

void setDBG(unsigned char b) {
    switch(b) {
        case 0:
            LATBbits.LATB0 = 1;
            break;
        case 1:
            LATBbits.LATB1 = 1;
            break;
        case 2:
            LATBbits.LATB2 = 1;
            break;
        case 3:
            LATBbits.LATB3 = 1;
            break;
        case 4:
            LATBbits.LATB4 = 1;
            break;
        default:
            break;
    }
}

void resetDBG(unsigned char b) {
    switch(b) {
        case 0:
            LATBbits.LATB0 = 0;
            break;
        case 1:
            LATBbits.LATB1 = 0;
            break;
        case 2:
            LATBbits.LATB2 = 0;
            break;
        case 3:
            LATBbits.LATB3 = 0;
            break;
        case 4:
            LATBbits.LATB4 = 0;
            break;
        default:
            break;
    }
}
void flipDBG(unsigned char b) {
    switch(b) {
        case 0:
            LATBbits.LATB0 = !LATBbits.LATB0;
            break;
        case 1:
            LATBbits.LATB1 = !LATBbits.LATB1;
            break;
        case 2:
            LATBbits.LATB2 = !LATBbits.LATB2;
            break;
        case 3:
            LATBbits.LATB3 = !LATBbits.LATB3;
            break;
        case 4:
            LATBbits.LATB4 = !LATBbits.LATB4;
            break;
        default:
            break;
    }
}
