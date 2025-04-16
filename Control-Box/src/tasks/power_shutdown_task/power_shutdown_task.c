#include "../../common/common.h"

Task_Handle powerShutdown_constructTask(){
    // Construct semaphore
    Semaphore_Params powerShutdownSemaphoreParams;
    Semaphore_Params_init(&powerShutdownSemaphoreParams);
    Semaphore_construct(&g_PowerShutdownSemaphoreStruct, 0, &powerShutdownSemaphoreParams);
    g_powerShutdownSemaphoreHandle = Semaphore_handle(&g_PowerShutdownSemaphoreStruct);

    // Enable power button interrupts and set callback
    GPIO_enableInt(CONFIG_PWR_BTN);
    GPIO_setCallback(CONFIG_PWR_BTN, powerShutdownISR);
    
    // Construct task
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_powerShutdownTaskStack;
    TaskParams.stackSize = POWER_SHUTDOWN_TASK_STACK_SIZE;
    TaskParams.priority = POWER_SHUTDOWN_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_PowerShutdownTaskStruct, powerShutdown_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_PowerShutdownTaskStruct;
}

void powerShutdown_executeTask(UArg arg0, UArg arg1){
    while (1){
        Task_sleep(g_taskSleepDuration);
        Semaphore_pend(g_powerShutdownSemaphoreHandle, BIOS_WAIT_FOREVER);

        printf("Turning power off...\n");
        clearAllPeripherals();
        destructAllResources();

        // Set power button to wake device from power shutdown state
        GPIO_setConfig(CONFIG_PWR_BTN, GPIO_CFG_IN_PU | GPIO_CFG_SHUTDOWN_WAKE_LOW);
        Power_shutdown(0, 0);

        printf("SHOULD NOT PASS THIS.\n");
        // If it does re-create all tasks for proper BIOS execution
        createAllResources();
    }
}

// Button_Handle buttonHandle, Button_EventMask buttonEvents
void powerShutdownISR(uint_least8_t index){
    printf("Button Pressed!\n");
    Semaphore_post(g_powerShutdownSemaphoreHandle);
}

void destructAllResources() {
    // Force close and unmounting of sd card before shutdown
    cleanupSDCard();
    
    // NOTE: ALL TASKS, SEMAPHORES, AND EVENTS MUST BE DESTRUCTED BEFORE "Power_shutdown()"" IS FORCED
    // ONCE THE MCU REBOOTS FROM A SHUTDOWN THEY MUST BE RE-CREATED FROM SCRATCH
    if (g_powerShutdownSemaphoreHandle != NULL){
        Semaphore_destruct(g_powerShutdownSemaphoreHandle);
        printf("Power shutdown semaphore destructed.\n");
    }
    if (g_powerShutdownTaskHandle != NULL){
        Task_destruct(g_powerShutdownTaskHandle);
        printf("Power shutdown task destructed.\n");
    }
    if (g_microSdWriteTaskHandle != NULL){
        Task_destruct(g_microSdWriteTaskHandle);
        printf("MicroSD write task destructed.\n");
    }
    if (g_temperatureMonitoringTaskHandle != NULL){
        Task_destruct(g_temperatureMonitoringTaskHandle);
        printf("Temperature monitoring task destructed.\n");
    }
    if (g_task1Handle != NULL){
        Task_destruct(g_task1Handle);
        printf("Task 1 destructed.\n");
    }
    if (g_displayDriverTaskHandle != NULL){
        Task_destruct(g_displayDriverTaskHandle);
        printf("Display driver task destructed.\n");
    }
    if (g_uartBridgeTaskHandle != NULL) {
        Task_destruct(g_uartBridgeTaskHandle);
        printf("UART bridge task destructed.\n");
    }
}

void clearAllPeripherals() {
    // Turn off LED's
    GPIO_write(6, 0);
    GPIO_write(7, 0);
}
