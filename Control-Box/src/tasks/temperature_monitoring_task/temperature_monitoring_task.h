#pragma once

// Define task parameters
#define TEMPERATURE_MONITORING_TASK_STACK_SIZE   1024
#define TEMPERATURE_MONITORING_TASK_PRIORITY     3
Task_Struct g_TemperatureMonitoringTaskStruct;
Task_Handle g_temperatureMonitoringTaskHandle;
uint8_t g_temperatureMonitoringTaskStack[TEMPERATURE_MONITORING_TASK_STACK_SIZE];

/**
* @brief Constructs the temperature monitoring task.
*
* This function initializes and creates the temperature monitoring task
* for the application. It sets up the task parameters such as stack size,
* priority, and arguments, and constructs the task using the TI-RTOS API.
*
* @return Task_Handle - A handle to the created temperature monitoring task.
*/
Task_Handle temperatureMonitoring_constructTask();

/**
* @brief Executes the temperature monitoring task.
*
* This function contains the main logic for the temperature monitoring task.
* It handles periodic temperature readings, adjusts task sleep duration
* based on predefined temperature thresholds, and logs the temperature
* for debugging purposes. The sleep duration adapts dynamically based on
* the current temperature state (default, high, or critical thresholds).
*
* @param arg0 - First argument passed to the task (not used).
* @param arg1 - Second argument passed to the task (not used).
*/
void temperatureMonitoring_executeTask(UArg arg0, UArg arg1);