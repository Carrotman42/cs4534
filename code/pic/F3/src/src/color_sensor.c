#include "maindefs.h"
#ifdef MASTER_PIC
#include "color_sensor.h"
#include "my_i2c.h"

static char config[2] = {0};

void initializeColorSensor(void){
    config[0] = 0x81;
    config[1] = 0xFF;

    i2c_master_send_no_raw(0x29,2,config);

    config[0] = 0x80;
    config[1] = 0x13;

    i2c_master_send_no_raw(0x29,2,config);

    config[0] = 0x84;
    config[1] = 0x02;

    i2c_master_send_no_raw(0x29,2,config);

    config[0] = 0x86;
    config[1] = 0x14;

    i2c_master_send_no_raw(0x29,2,config);

    config[0] = 0x8C;
    config[1] = 0x02;

    i2c_master_send_no_raw(0x29,2,config);

    config[0] = 0xE6;

    i2c_master_send_no_raw(0x29,1,config);


}

//void initializeColorSensor(uint16 upperThresh, uint16 lowerThresh, char persistence){
//    config[0] = 0x81;
//    config[1] = 0xFF;
//
//    config[0] = 0x80;
//    config[1] = 0x13;
//
//    config[0] = 0x84;
//    config[1] = 0x02;
//
//    config[0] = 0x86;
//    config[1] = 0x14;
//
//    config[0] = 0x8C;
//    config[1] = 0x02;
//
//    config[0] = 0xE6;
//}

void clearColorSensorInterrupt(void){

    config[0] = 0xE6;

    i2c_master_send_no_raw(0x29,1,config);

}

#endif
