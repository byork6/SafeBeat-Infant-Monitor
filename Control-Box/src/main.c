#include "common.h"
#include "tasks/test_gpio_task/test_gpio_task.h"

void *mainThread(void *arg0){
    // For testing GPIO pins one at a time without using RTOS
    // testGPIO(5);

    /////// PROJECT CODE ///////
    // Init local variables
    uint32_t time = 1;
    printVar("time", &time, 'd');

    // Call TI driver initializations
    initBOARD();

    // Call custom board configurations
    configBOARD();

    // Create tasks for TI-RTOS
    // Task 1
    uint32_t pinToTest = 6;
    uint32_t taskPriority = 1;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 2
    pinToTest = 7;
    taskPriority = 1;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 3
    // TODO: RF Recieve task init goes here

    // TODO: Add Any more tasks here

    // Start TI-RTOS BIOS execution
    BIOS_start();

    return NULL;
}
