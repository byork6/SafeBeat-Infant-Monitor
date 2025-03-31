#include "../../common/common.h"

// Define global variable for spi handle --- declared in common.h using extern
SPI_Handle g_spiDisplayHandle = NULL;

Task_Handle displayDriver_constructTask(){
    // Construct task
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_displayDriverTaskStack;
    TaskParams.stackSize = DISPLAY_DRIVER_TASK_STACK_SIZE;
    TaskParams.priority = DISPLAY_DRIVER_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_DisplayDriverTaskStruct, displayDriver_executeTask, &TaskParams, NULL);
    g_displayDriverTaskHandle = (Task_Handle)&g_DisplayDriverTaskStruct;
    return (Task_Handle)&g_DisplayDriverTaskStruct;
}

void displayDriver_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;
    int i = 0;

    // Init SPI
    SPI_Params spiParams;
    SPI_Params_init(&spiParams);
    g_spiDisplayHandle = SPI_open(CONFIG_DISPLAY_SPI, &spiParams);

    printf("Entering displayDriver_executeTask()...\n");

    while (1) {
        i++;
        printf("Display driver Count: %d\n", i);

        if (g_spiDisplayHandle == NULL){
            printf("Display SPI not initialized.\n");
            Task_sleep(g_taskSleepDuration);
            continue;
        }

        // Placeholder test values — replace with real data later
        int testHeartRate = 110;
        int testRespirationRate = 35;

        // TODO: add code here

        Task_sleep(g_taskSleepDuration);
    }
}

void renderDisplay(int heartRate, int respirationRate) {
    printf("HR: %d  |  RR: %d\n", heartRate, respirationRate);

    if (heartRate < LOW_HEART_RATE_THRESHOLD_BPM || heartRate > HIGH_HEART_RATE_THRESHOLD_BPM ||
        respirationRate < LOW_RESPIRATORY_RATE_THRESHOLD_BRPM || respirationRate > HIGH_RESPIRATORY_RATE_THRESHOLD_BRPM) {
        printf("ALARM: Abnormal vitals detected!\n");
    }

    // We need to replace printf() with FT813 draw-text commands once graphics integration starts
}
