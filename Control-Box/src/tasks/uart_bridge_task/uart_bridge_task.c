#include "../../common/common.h"

// TYPE DEFINITIONS

// VARIABLE DEFINITIONS

Task_Handle uartBridge_constructTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = g_uartBridgeTaskStack;
    // Stack array size
    TaskParams.stackSize = UART_BRIDGE_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = UART_BRIDGE_TASK_PRIORITY;
    // arg0 and ar1 passed to exectution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_uartBridgeTaskStruct, uartBridge_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_uartBridgeTaskStruct;
}

void uartBridge_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    int i = 0;

    printf("Entering uartBridge_executeTask()...\n");
    printf("uartBridge Initialized.\n");
    while(1){
        i++;
        printf("uartBridge Count: %d\n", i);
        
        // TODO: Runtime code goes here

        Task_sleep(g_taskSleepDuration);
    }
}
