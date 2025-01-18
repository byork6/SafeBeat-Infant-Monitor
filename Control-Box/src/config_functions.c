#include "common.h"
#include "ti/drivers/SD.h"

// Declare variables before init calls
char fatfsPrefix[] = "fat";

void initBOARD(void){
    GPIO_init();
    SPI_init();
    SD_init();
    SDFatFS_init();
    add_device(fatfsPrefix, _MSA, ffcio_open, ffcio_close, ffcio_read, ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);
    // Display_init();  // Optional: API to print info to a UART terminal, must include Display.h in common 
                        // and set up disply driver in the .sysconfig
}

void configBOARD(void){
    configGPIO();
    // configSPI();
    // TODO: Add config calls as needed
}

void configGPIO(void){
    // TODO: Set up initial config for GPIO pins
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
    // TODO: Set up initial config for SPI pins
}