#include "common.h"

Task_Handle displayDriver_constructTask(){
    // Construct task
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_displayDriverTaskStack;
    TaskParams.stackSize = DISPLAY_DRIVER_TASK_STACK_SIZE;
    TaskParams.priority = DISPLAY_DRIVER_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_DisplayDriverTaskStruct, displayDriver_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_DisplayDriverTaskStruct;
}

void displayDriver_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    int i  = 0;

    printf("Entering displayDriver_executeTask()...\n");
    while (1){
        i++;
        printf("displayDriver Count: %d\n", i);

        // TODO: Add display driver code here.

        Task_sleep(g_taskSleepDuration);
    }
}
