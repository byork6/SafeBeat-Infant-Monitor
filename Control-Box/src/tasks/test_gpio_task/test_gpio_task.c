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
    printf("Entering testGpio_executeTask()...\n");

    GPIO_setConfig(arg0, GPIO_SET_OUT_AND_DRIVE_LOW);
    int i  = 0;
    
    printf("TestGpio Initialized.\n");
    while (1){
        i++;
        printf("testGpio Count: %d\n", i);
        GPIO_toggle(arg0);
        Task_sleep(g_taskSleepDuration);
    }
}
