#include "common/common.h"
#include "ti/sysbios/BIOS.h"

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
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will execute the power policy. 
    // If the powerbutton is pushed Power_shutdown() will be forced.
    Power_enablePolicy();

    Types_FreqHz cpuFreq;
    uint32_t clockTicks = 0;
    BIOS_getCpuFreq(&cpuFreq);
    printVar("Starting cpu frequency:", &cpuFreq.lo, 'U');
    cpuFreq.lo = cpuFreq.lo / 2048;
    printVar("Set CPU frequency: ", &cpuFreq.lo, 'U');
    BIOS_setCpuFreq(&cpuFreq);
    clockTicks = Clock_getTickPeriod();
    printVar("Starting clock tick period:", &clockTicks, 'U');
    // TODO: Divide clock period and see if it slows down MCU.



    createAllResources();

    return NULL;
}
