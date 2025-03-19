#include "../../common/common.h"

Task_Handle bleCentral_constructTask(){
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_bleCentralTaskStack;
    TaskParams.stackSize = BLE_CENTRAL_TASK_STACK_SIZE;
    TaskParams.priority = BLE_CENTRAL_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_BleCentralTaskStruct, bleCentral_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_BleCentralTaskStruct;
}

void bleCentral_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    while (1) {

    }
}
