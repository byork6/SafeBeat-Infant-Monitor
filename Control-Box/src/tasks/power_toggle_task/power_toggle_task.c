#include "../../common/common.h"
#include "power_toggle_task.h"

volatile bool g_isSystemOff = false;

void powerToggle_createTask(){
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set paramerters.
    Task_Params_init(&TaskParams);
    TaskParams.stack = g_powerToggleTaskStack;
    TaskParams.stackSize = POWER_TOGGLE_TASK_STACK_SIZE;
    TaskParams.priority = POWER_TOGGLE_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_PowerToggleTaskStruct, powerToggle_executeTask, &TaskParams, NULL);
}

void powerToggle_executeTask(UArg arg0, UArg arg1){
    printStr("Entering power toggle");
    while(1){
        Semaphore_pend(g_powerToggleSemaphore, BIOS_WAIT_FOREVER);
        if(g_isSystemOff){
            printStr("Turning power on...");
            Power_setConstraint(PowerCC26XX_DISALLOW_SHUTDOWN);
            // resume all tasks fxn here
            g_isSystemOff = false;
            printStr("System state set to on.");
        }
        else{
            // TODO: Problem - Never gets out of else statement here. Result - Button to turn system back on is unresponsive
            printStr("Turning power off...");
            // suspend all tasks fxn here
            Power_releaseConstraint(PowerCC26XX_DISALLOW_SHUTDOWN);
            uint_fast16_t shutdownState = 0;
            uint_fast32_t shutdownTime = 0;
            Power_shutdown(shutdownState, shutdownTime);
            g_isSystemOff = true;
            printStr("System state set to off.");
        }
    }
}

void powerToggleISR(uint_least8_t index){
    printStr("Button Pressed!");
    Semaphore_post(g_powerToggleSemaphore);
}
