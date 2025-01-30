#include "../common/common.h"

void initBOARD(void){
    Power_init();
    GPIO_init();
    SPI_init();
    SDFatFS_init();
    Temperature_init();
    // NOTE: Add init drivers as needed. Init functions called here must also be selected in the .syscfg file for the project.
    
    // NOTE: We may not need custom calls for init since .sysconfig handles it -- commented out custom calls for now.
    // configBOARD();
}

void configBOARD(void){
    // CUSTOM CONFIG CALLS BELOW GO HERE
    configGPIO();
    // configSPI();
}

void configGPIO(void){
    // GPIO CUSTOM INIT HERE
    // Set GPIO Pins 5, 7-17 as output and drive low on startup for LCD.
    GPIO_setConfig(5, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(6, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(7, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(8, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(9, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(10, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(11, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(12, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(13, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(14, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(15, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(16, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_setConfig(17, GPIO_SET_OUT_AND_DRIVE_LOW);
}

void configSPI(void){
    // SPI CUSTOM INIT HERE
}