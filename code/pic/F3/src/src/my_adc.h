#ifndef MY_ADC_H
#define	MY_ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "maindefs.h"
#ifdef SENSOR_PIC
#define IRBUFFERSIZE 5
#define HALFBUFFER IRBUFFERSIZE/2

    typedef struct{
        uint8 ir0Array[IRBUFFERSIZE];
        uint8 ir1Array[IRBUFFERSIZE];
        uint8 count;
    } irBuffer;
    
    void init_adc();
    void adc_int_handler();
    void addDataToBuffer(char ir0Data, char ir1Data);
    void sort(uint8* array);
    char* transmitData();
    void calculateDistance();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* MY_ADC_H */

