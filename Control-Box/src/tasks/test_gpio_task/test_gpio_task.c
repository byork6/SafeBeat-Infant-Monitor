#include "../../common/common.h"

Task_Handle testGpio_constructTask(uint32_t pinNumber, uint32_t taskPriority, Task_Struct *taskStruct, uint8_t *taskStack){
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
    return (Task_Handle)taskStruct;
}

void testGpio_executeTask(UArg arg0, UArg arg1){
    (void)arg1;     // suppresses warnings for unused arg
    printStr("Entering testGpio_executeTask()");

    GPIO_setConfig(arg0, GPIO_SET_OUT_AND_DRIVE_LOW);
    int i  = 0;
    
    printStr("testGpio Initialized...");
    while (1){
        i++;
        printVar("testGpio Count: ", &i, 'd');
        GPIO_toggle(arg0);
        logData(120, 20, "12:30:00 02/10/2025");
        logData(121, 20, "12:30:01 02/10/2025");
        logData(122, 20, "12:30:02 02/10/2025");
        logData(123, 20, "12:30:03 02/10/2025");
        Task_sleep(g_taskSleepDuration);
    }
}
