#pragma once

// Define task parameters
#define POWER_TOGGLE_TASK_STACK_SIZE   1024
#define POWER_TOGGLE_TASK_PRIORITY     1
Task_Struct g_PowerToggleTaskStruct;
Task_Handle g_powerTaskHandle;
uint8_t g_powerToggleTaskStack[POWER_TOGGLE_TASK_STACK_SIZE];

// Define semaphore parameters
Semaphore_Handle g_powerToggleSemaphore;

/**
* @brief Constructs the power toggle task.
*
* This function initializes and creates the power toggle task
* for the application. It sets up the task parameters such as 
* stack size, priority, and arguments, and constructs the task
* using the TI-RTOS API.
*
* @return Task_Handle - A handle to the created power toggle task.
*/
Task_Handle powerShutdown_createTask();

/**
* @brief Executes the power toggle task.
*
* This function contains the main logic for the power toggle task.
* It handles the task's execution, including checking for semaphore
* signals, performing power shutdown, and re-creating tasks upon wake-up.
*
* @param arg0 - First argument passed to the task (not used).
* @param arg1 - Second argument passed to the task (not used).
*/
void powerShutdown_executeTask(UArg arg0, UArg arg1);

/**
* @brief Interrupt Service Routine (ISR) for the power toggle button.
*
* This ISR is triggered when the power toggle button is pressed. It
* posts a semaphore to signal the power toggle task to perform its
* operations.
*
* @param index - The index of the GPIO pin triggering the ISR.
*/
void powerToggleISR(uint_least8_t index);

/**
* @brief Destructs all tasks and releases resources before shutdown.
*
* This function destructs all tasks, semaphores, and other RTOS
* resources to ensure a proper shutdown. These resources must
* be re-created upon wake-up or reboot.
*/
void destructAllTasks();
