#include "error.h"
#if defined(PICMAN) || defined(MOTOR_PIC) || defined(SENSOR_PIC)
static uint8 err = 0x00; //we only need to save 1 byte of param - the latest error we've seen

void saveError(uint8 param){
    err = param;
}


void makeErrorHeaderIfNeeded(uint8 length, char* packedFrameMessage){
    if((length == 0) || (err == 0x00)){
        return;
    }
    generateErrorFromParam(packedFrameMessage, length, err, I2C_COMM);//always I2C no matter where.  UART contains no frames
    err = 0x00; //reset err now that we're sending it out
}
#endif