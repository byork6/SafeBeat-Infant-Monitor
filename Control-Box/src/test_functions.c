#include "common.h"
#include "test_functions.h"


void testGPIO(uint32_t pin_config_index){
    //////////////// TEST CODE  ONLY ////////////////
    
    // Input validation (Pins 5-30 only valid pins)
    if (pin_config_index < 5 || pin_config_index > 30){
        exit(1);
    }

    // 1 second delay
    uint32_t time = 1;

    // Call driver init functions from SDK
    GPIO_init();

    // Initialize GPIO pins
    GPIO_setConfig(pin_config_index, GPIO_SET_OUT_AND_DRIVE_LOW);

    while (1)
    {
        sleep(time);
        GPIO_toggle(pin_config_index);
    }
}

void testBLE5(){
    /*TODO: write code that connects MCU to SimpleLink app based off "project_zero example". 
    *       The ble connection should toggle LED when app writes to the MCU
    */
}
