#include "../../common.h"
#include "test_gpio_task.h"

void testGpio_createTask(uint32_t pinNumber, uint32_t taskPriority, Task_Struct *taskStruct, uint8_t *taskStack){
    // Declare TaskParams struct name
    Task_Params TaskParams;


    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = taskStack;
    // Stack array size
    TaskParams.stackSize = TEST_GPIO_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = taskPriority;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = pinNumber;
    TaskParams.arg1 = 0;    // 0 used if arg is unused.

    // Construct the TI-RTOS task using the API
    Task_construct(taskStruct, testGpio_executeTask, &TaskParams, NULL);
}

void testGpio_executeTask(UArg arg0, UArg arg1){
    (void)arg1;     // suppresses warnings for unused arg
    printStr("Entering testGpio_executeTask()");

    GPIO_setConfig(arg0, GPIO_SET_OUT_AND_DRIVE_LOW);
    int i  = 0;
    
    printStr("testGpio Initialized...");
    while (1){
        GPIO_toggle(arg0);  
        printVar("testGpio Count: ", &i, 'd');
        if (i == 0){
            Task_sleep(500);
        }
        i++;

        // Clock_tickPeriod = 10 us --- delayTime in seconds
        // Task_sleep(1000);
        Task_yield();
    }
}
