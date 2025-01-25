#include "../../common/common.h"

Task_Handle powerShutdown_createTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    TaskParams.stack = g_powerShutdownTaskStack;
    TaskParams.stackSize = POWER_SHUTDOWN_TASK_STACK_SIZE;
    TaskParams.priority = POWER_SHUTDOWN_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_PowerShutdownTaskStruct, powerShutdown_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_PowerShutdownTaskStruct;
}

void powerShutdown_executeTask(UArg arg0, UArg arg1){
    while (1){
        Task_sleep(g_taskSleepDuration);
        Semaphore_pend(g_powerShutdownSemaphore, BIOS_WAIT_FOREVER);

        printStr("Turning power off...");
        clearAllPeripherals();
        destructAllTasks();

        // Set power button to wake device from power shutdown state
        GPIO_setConfig(CONFIG_GPIO_PWR_BTN, GPIO_CFG_IN_PU | GPIO_CFG_SHUTDOWN_WAKE_LOW);
        Power_shutdown(0, 0);

        printStr("SHOULD NOT PASS THIS.");
        // If it does re-create all tasks for proper BIOS execution
        createAllResources();
    }
}

// Button_Handle buttonHandle, Button_EventMask buttonEvents
void powerShutdownISR(uint_least8_t index){
    printStr("Button Pressed!");
    Semaphore_post(g_powerShutdownSemaphore);
}

void destructAllTasks() {
    // NOTE: ALL TASKS, SEMAPHORES, AND EVENTS MUST BE DESTRUCTED BEFORE "Power_shutdown()"" IS FORCED
    // ONCE THE MCU REBOOTS FROM A SHUTDOWN THEY MUST BE RE-CREATED FROM SCRATCH
    if (g_task1Handle != NULL) {
        Task_destruct(g_task1Handle);
        printStr("Task 1 destructed.");
    }
    if (g_task2Handle != NULL) {
        Task_destruct(g_task2Handle);
        printStr("Task 2 destructed.");
    }
    if (g_powerTaskHandle != NULL){
        Task_destruct(g_powerTaskHandle);
        printStr("Power task destructed.");
    }
    if (g_powerShutdownSemaphore != NULL){
        Semaphore_destruct(g_powerShutdownSemaphore);
        printStr("Power semaphore destructed.");
    }
}

void clearAllPeripherals() {
    // Turn off LED's
    GPIO_write(6, 0);
    GPIO_write(7, 0);
}
