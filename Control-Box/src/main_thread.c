#include "common.h"

void *mainThread(void *arg0){

    // INIT POWER
    // Retrieve power-reset reason
    PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // If we are waking up from shutdown, we do something extra.
    if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
        // Application code must always disable the IO latches when coming out of shutdown
        PowerCC26X2_releaseLatches();
    }

    // INIT BLE
    /* Register Application callback to trap asserts raised in the Stack */
    RegisterAssertCback(AssertHandler);

    // Enable iCache prefetching
    VIMSConfigure(VIMS_BASE, TRUE, TRUE);

    // Enable cache
    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);

    // INIT BOARD
    // Initialize the board with TI-Driver configurations based on main.syscfg generated files.
    initBOARD();
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will execute the power policy. 
    // If the powerbutton is pushed Power_shutdown() will be forced.
    Power_enablePolicy();
    
    // INIT TASKS
    // Constructs all RTOS tasks before BIOS_start() is called in main_tirtos.c
    constructAllResources();

    // RETURNS TO BIOS_START()
    return NULL;
}
