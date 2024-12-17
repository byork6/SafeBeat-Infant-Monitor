#include "common.h"
#include "test_functions.h"

void testGpio_createTask(uint32_t pinNumber){
    // Declare TaskParams struct name
    Task_Params TaskParams;


    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = g_gpioTaskStack;
    // Stack array size
    TaskParams.stackSize = GPIO_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = GPIO_TASK_PRIORITY;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = pinNumber;
    TaskParams.arg1 = 0;    // 0 used if arg is unused.

    /**
    * @brief  Statically constructs and initializes a TI-RTOS task.
    *
    * This function creates a statically allocated task using the Task_construct API. 
    * It sets up the task function, task stack, priority, and optional arguments 
    * needed for execution. Task_construct is ideal for real-time applications where 
    * dynamic memory allocation is undesirable. Task_create() is an alternate function
    * call that uses dynamic memory allocation instead.
    *
    * @param taskStruct     Pointer to a Task_Struct object that will hold the task's 
    *                       control information. This must be statically allocated.
    * @param taskFunction   Function pointer to the task's entry function. The function 
    *                       must have the signature: void (UArg arg0, UArg arg1).
    * @param taskParams     Pointer to a Task_Params structure containing task 
    *                       configuration, such as stack size, stack location, 
    *                       task priority, and task arguments.
    * @param eb             Pointer to an Error_Block for error handling. If not used, 
    *                       this can be set to NULL.
    *
    * @note The task function is executed when the TI-RTOS scheduler starts, and it 
    *       runs according to the specified priority and stack configuration.
    *
    * @return None
    */
    Task_construct(&GpioTaskStruct, testGpio_executeTask, &TaskParams, NULL);
}

void testGpio_executeTask(UArg arg0, UArg arg1){
    uint32_t pinNumber = (uint32_t)arg0;  

    GPIO_setConfig(pinNumber, GPIO_SET_OUT_AND_DRIVE_LOW);

    while (1)
    {
        GPIO_toggle(pinNumber);  
        // Clock_tickPeriod = 10 us --- delayTime in seconds
        Task_sleep(DELAY_US(DELAY_DURATION_US)); 
    }
}

void testGpio(uint32_t pin_config_index){
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
