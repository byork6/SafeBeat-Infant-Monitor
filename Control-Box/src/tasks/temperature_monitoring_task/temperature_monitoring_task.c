#include "../../common/common.h"

Task_Handle temperatureMonitoring_constructTask(){
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
    int16_t currentTemp = 0;

    while(1){
        // Get current temperature for debugging
        currentTemp = Temperature_getTemperature();
        printVar("Current temperature in celsius:", &currentTemp, 'I');

        // Adjust sleep duration based on temperature thresholds
        if (currentTemp >= CRITICAL_TEMP_THRESHOLD_CELSIUS) {
            if (g_taskSleepDuration != CRITICAL_TEMP_TASK_SLEEP_DURATION){
                g_taskSleepDuration = CRITICAL_TEMP_TASK_SLEEP_DURATION;
            }
        } 
        else if (currentTemp >= HIGH_TEMP_THRESHOLD_CELSIUS) {
            if (g_taskSleepDuration != HIGH_TEMP_TASK_SLEEP_DURATION){
                g_taskSleepDuration = HIGH_TEMP_TASK_SLEEP_DURATION;
            }
        } 
        else {
            if (g_taskSleepDuration != DEFAULT_TASK_SLEEP_DURATION){
                g_taskSleepDuration = DEFAULT_TASK_SLEEP_DURATION;
            }
        }

        Task_sleep(TEMP_MONITORING_TASK_SLEEP_DURATION);
    }
}
