#include "common/common.h"

void *mainThread(void *arg0){

    // Retrieve power-reset reason
    PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // If we are waking up from shutdown, we do something extra.
    if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
        // Application code must always disable the IO latches when coming out of shutdown
        PowerCC26X2_releaseLatches();
    }

    // Initialize the board with TI-Driver config & custom config if needed
    initBOARD();
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will executre power policy or if the powerbutton is pushed Power_shutdown() will be forced.
    Power_enablePolicy();

    createAllResources();

    return NULL;
}
