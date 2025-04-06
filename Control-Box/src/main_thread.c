#include "common.h"

void *mainThread(void *arg0){

    // INIT POWER
    // Retrieve power-reset reason
    printf("Init Reset Reason\n");
    PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // If we are waking up from shutdown, we do something extra.
    printf("Init Latches\n");
    if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
        // Application code must always disable the IO latches when coming out of shutdown
        PowerCC26X2_releaseLatches();
    }

    // INIT BLE
    /* Register Application callback to trap asserts raised in the Stack */
    printf("Init assert callback\n");
    RegisterAssertCback(AssertHandler);

    // Enable iCache prefetching
    printf("Init VIMS\n");
    VIMSConfigure(VIMS_BASE, TRUE, TRUE);

    // Enable cache
    printf("Init VIMS Mode\n");
    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);

    // INIT BOARD
    // Initialize the board with TI-Driver configurations based on main.syscfg generated files.
    printf("Init Board\n");
    initBOARD();
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will execute the power policy. 
    // If the powerbutton is pushed Power_shutdown() will be forced.
    printf("Enable power policy\n");
    Power_enablePolicy();
    
    // INIT TASKS
    // Constructs all RTOS tasks before BIOS_start() is called in main_tirtos.c
    printf("Construct Resources\n");
    constructAllResources();

    // RETURNS TO BIOS_START()
    printf("BIOS Starting...\n");
    return NULL;
}
