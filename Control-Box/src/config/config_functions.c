#include "common.h"

void initBOARD(void){
    // Call TI init functions, these use the pin configurations from .syscfg generated files and negate the need to write custom pin inits.
    Power_init();
    GPIO_init();
    SPI_init();
    SDFatFS_init();
    Temperature_init();
    ICall_init();
    ICall_createRemoteTasks();
    ECDH_init();
    AESECB_init();
    AESCCM_init();
    TRNG_init();
    AESCTRDRBG_init();
}

void constructAllResources() {
    // Create tasks for TI-RTOS7 --- Order them from lowest to highest priority.
    // Task 1 --- Priority = 1
    g_powerShutdownTaskHandle = powerShutdown_constructTask();

    // Task 2 --- Priority = 2
    g_microSdWriteTaskHandle = microSdWrite_constructTask();

    // Task 3 --- Priority = 3
    g_displayDriverTaskHandle = displayDriver_constructTask();

    // Task 4 --- Priority = 4
    g_bleCentralTaskHandle = bleCentral_constructTask();
    
    // Task 4 --- Priority = 5
    g_task1Handle = testGpio_constructTask(6, RED_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 5 --- Priority = 5
    g_task2Handle = testGpio_constructTask(7, GREEN_LIGHT_BLINK_PRIORITY, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 6 --- Priority = 6
    g_temperatureMonitoringTaskHandle = temperatureMonitoring_constructTask();
}
