#include "../../common/common.h"

Task_Handle temperatureMonitoring_createTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    TaskParams.stack = g_temperatureMonitoringTaskStack;
    TaskParams.stackSize = TEMPERATURE_MONITORING_TASK_STACK_SIZE;
    TaskParams.priority = TEMPERATURE_MONITORING_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_TemperatureMonitoringTaskStruct, temperatureMonitoring_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_TemperatureMonitoringTaskStruct;
}

void temperatureMonitoring_executeTask(UArg arg0, UArg arg1){
    // TODO: Create the task here and make sure to implement into power shutdown framework.
}
