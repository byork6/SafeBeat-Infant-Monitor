#pragma once

// Define task parameters
#define TEMPERATURE_MONITORING_TASK_STACK_SIZE   1024
#define TEMPERATURE_MONITORING_TASK_PRIORITY     3
Task_Struct g_TemperatureMonitoringTaskStruct;
Task_Handle g_temperatureMonitoringTaskHandle;
uint8_t g_temperatureMonitoringTaskStack[TEMPERATURE_MONITORING_TASK_STACK_SIZE];

Task_Handle temperatureMonitoring_createTask();

void temperatureMonitoring_executeTask(UArg arg0, UArg arg1);