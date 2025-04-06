#include "common.h"

void initBOARD(void){
    // Call TI init functions, these use the pin configurations from .syscfg generated files and negate the need to write custom pin inits.
    printf("Power_init\n");
    Power_init();
    printf("GPIO_init\n");
    GPIO_init();
    printf("SPI_init\n");
    SPI_init();
    printf("SDFatFS_init\n");
    SDFatFS_init();
    printf("Temperature_init\n");
    Temperature_init();
    printf("ICall_init\n");
    ICall_init();
    printf("ICall_createRemoteTasks\n");
    ICall_createRemoteTasks();
    // All of these below are use by the example --- Optional
    // printf("ECDH_init\n");
    // ECDH_init();
    // printf("AESECB_init\n");
    // AESECB_init();
    // printf("AESCCM_init\n");
    // AESCCM_init();
    // printf("TRNG_init\n");
    // TRNG_init();
    // printf("AESCTRDRBG_init\n");
    // AESCTRDRBG_init();
    // printf("BOARD INIT FUNCTIONS COMPLETED.\n");
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
