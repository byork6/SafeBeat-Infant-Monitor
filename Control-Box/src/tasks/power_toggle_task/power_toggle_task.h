#pragma once

// Define task parameters
#define POWER_TOGGLE_TASK_STACK_SIZE   1024
#define POWER_TOGGLE_TASK_PRIORITY     1
Task_Struct g_PowerToggleTaskStruct;
uint8_t g_powerToggleTaskStack[POWER_TOGGLE_TASK_STACK_SIZE];

// Declare semaphore handles for power-button hwi
Semaphore_Handle g_powerToggleSemaphore;

void powerToggle_createTask();

void powerToggle_executeTask(UArg arg0, UArg arg1);

void powerToggleISR(uint_least8_t index);

void suspendAllTasks();

void resumeAllTasks();
