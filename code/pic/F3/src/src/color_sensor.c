#include "maindefs.h"
#ifdef MASTER_PIC
#include "color_sensor.h"
#include "my_i2c.h"

uint8 colorSensorInitStage = 0;

void initializeColorSensor(void){
    for(int i = 0; i < 10000; i++);
    colorSensorInitStage = 0;
    initializeColorSensorStage(); //start at stage 0
}

void initializeColorSensorStage(){
    char config[2] = {0};
    switch(colorSensorInitStage){
        case ENABLE:
            config[0] = 0x81;
            config[1] = 0xFF;
            break;
        case INT_ENABLE:
            config[0] = 0x80;
            config[1] = 0x13;
            break;
        case THRESHOLD_LOW:
            config[0] = 0x84;
            config[1] = 0x02;
            break;
        case THRESHOLD_HIGH:
            config[0] = 0x86;
            config[1] = 0x14;
            break;
        case PERSISTENCE:
            config[0] = 0x8C;
            config[1] = 0x02;
            break;
        case INT_CLEAR:
            clearColorSensorInterrupt();
            break;
        default:
            break;
    }
    if(colorSensorInitStage != INT_CLEAR){
        i2c_master_send_no_raw(COLOR_SENSOR_ADDR, 2, config);
    }
    colorSensorInitStage++;
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
    char config[1] = {0};
    config[0] = 0xE6;

    i2c_master_send_no_raw(COLOR_SENSOR_ADDR,1,config);

}

#endif
