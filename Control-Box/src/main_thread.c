#include "common/common.h"
#include "tasks/microSD_write_task/microSD_write_task.h"
#include "tasks/test_gpio_task/test_gpio_task.h"
#include "tasks/power_toggle_task/power_toggle_task.h"
#include "ti/drivers/Power.h"
#include "ti/drivers/power/PowerCC26XX.h"
#include "ti_drivers_config.h"

void *mainThread(void *arg0){
    // Initialize the board with TI-Driver config & custom config if needed
    initBOARD();
    Power_enablePolicy();

    printStr("Turning power on...");

    // TODO: The block below only lets tasks execute the first time, but it is required to allow button to wake device from power down.
    // ///////////////////////////////////////////////////////
    // PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // /* If we are waking up from shutdown, we do something extra. */
    // if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
    //     /* Application code must always disable the IO latches when coming out of shutdown */
    //     PowerCC26X2_releaseLatches();
    //     printStr("Coming out of shutdown state, IO latches disabled.");
    // }
    ///////////////////////////////////////////////////////////

    Semaphore_Params powerToggleSemaphoreParams;
    Semaphore_Params_init(&powerToggleSemaphoreParams);
    g_powerToggleSemaphore = Semaphore_create(0, &powerToggleSemaphoreParams, NULL);

    GPIO_enableInt(CONFIG_GPIO_PWR_BTN);
    GPIO_setCallback(CONFIG_GPIO_PWR_BTN, powerToggleISR);

    powerToggle_createTask();

    // Create tasks for TI-RTOS7
    // Task 1
    uint32_t pinToTest = 6;
    uint32_t taskPriority = 3;
    task1Handle = testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // // Task 2
    pinToTest = 7;
    taskPriority = 3;
    task2Handle = testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 3
    // TODO: Test microSD Driver with physical connection
    // microSDWrite_createTask();

    return NULL;
}
