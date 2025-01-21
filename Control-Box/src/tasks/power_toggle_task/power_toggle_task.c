#include "../../common/common.h"
#include "ti/drivers/GPIO.h"
#include "ti/drivers/power/PowerCC26XX.h"
#include "ti/sysbios/BIOS.h"
#include "ti_drivers_config.h"
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
    // TODO: Resume and suspend tasks are entered but tasks never pause/resume.
    printStr("POWER STATUS IS ACTIVE");
    Semaphore_pend(g_powerToggleSemaphore, BIOS_WAIT_FOREVER);
    printStr("Turning power off...");
    GPIO_setConfig(CONFIG_GPIO_PWR_BTN, GPIO_CFG_IN_PU | GPIO_CFG_SHUTDOWN_WAKE_LOW);
    Power_shutdown(0, 0);
    // Power_sleep(PowerCC26XX_STANDBY);
    // Power_idleFunc();
    printStr("SHOULD NOT PASS THIS.");
    while(1){
        if(g_isSystemOff){
            printStr("Turning power on...");
            // Task_enable();
            // Power_setConstraint(PowerCC26XX_DISALLOW_SHUTDOWN);

            // // Check for power constraints
            // constraints = Power_getConstraintMask();
            // printf("Active constraints: 0x%x\n", constraints);

            // if (constraints & PowerCC26XX_DISALLOW_STANDBY) {
            //     printf("Standby is disallowed.\n");
            // } else {
            //     printf("Standby is allowed.\n");
            // }
            // resumeAllTasks();
            g_isSystemOff = false;
            printStr("System state set to on.");
        }
        else{
            printStr("Turning power off...");
            // Task_disable();
            // suspendAllTasks();
            // Power_releaseConstraint(PowerCC26XX_DISALLOW_SHUTDOWN);
            // printStr("Disallow shutdown constraint released.");

            // // Check for power constraints
            // constraints = Power_getConstraintMask();
            // printf("Active constraints: 0x%x\n", constraints);

            // if (constraints & PowerCC26XX_DISALLOW_STANDBY) {
            //     printf("Standby is disallowed.\n");
            // } else {
            //     printf("Standby is allowed.\n");
            // }

            // Power_sleep(PowerCC26XX_STANDBY);
            g_isSystemOff = true;
            printStr("System state set to off.");
        }
    }
}

// Button_Handle buttonHandle, Button_EventMask buttonEvents
void powerToggleISR(uint_least8_t index){
    printStr("Button Pressed!");
    Semaphore_post(g_powerToggleSemaphore);
}

void suspendAllTasks() {
    if (task1Handle != NULL) {
        Task_block(task1Handle);
        printStr("Task 1 suspended.");
    }
    if (task2Handle != NULL) {
        Task_block(task2Handle);
        printStr("Task 2 suspended.");
    }
}

void resumeAllTasks() {
    if (task1Handle != NULL) {
        Task_unblock(task1Handle);
        printStr("Task 1 resumed.");
    }
    if (task2Handle != NULL) {
        Task_unblock(task2Handle);
        printStr("Task 2 resumed.");
    }
}
