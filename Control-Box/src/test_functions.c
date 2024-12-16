#include "common.h"
#include "test_functions.h"

void testGPIO_createTask(void){
  Task_Params taskParams;

  Task_Params_init(&taskParams);
  taskParams.stack = gpioTaskStack;
  taskParams.stackSize = GPIO_TASK_STACK_SIZE;
  taskParams.priority = GPIO_TASK_PRIORITY;
  taskParams.arg0 = 6;

  Task_construct(&gpioTask, testGPIO_executeTask, &taskParams, NULL);
}

void testGPIO_executeTask(UArg arg0, UArg arg1){
    uint32_t pin = (uint32_t)arg0;  

    GPIO_setConfig(pin, GPIO_SET_OUT_AND_DRIVE_LOW);

    while (1)
    {
        GPIO_toggle(pin);  
        // Clock_tickPeriod = 10 us --- delayTime in seconds
        Task_sleep(DELAY_US(DELAY_DURATION_US)); 
    }
}

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
