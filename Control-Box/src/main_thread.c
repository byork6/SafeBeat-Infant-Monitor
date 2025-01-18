#include "common/common.h"
#include "tasks/microSD_write_task/microSD_write_task.h"
#include "tasks/test_gpio_task/test_gpio_task.h"

void *mainThread(void *arg0){
    // Initialize the board with TI-Driver config & custom config if needed
    initBOARD();

    // Create tasks for TI-RTOS7
    // Task 1
    uint32_t pinToTest = 6;
    uint32_t taskPriority = 2;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct1, (uint8_t *)g_testGpioTaskStack1);

    // Task 2
    pinToTest = 7;
    taskPriority = 2;
    testGpio_createTask(pinToTest, taskPriority, &g_TestGpioTaskStruct2, (uint8_t *)g_testGpioTaskStack2);

    // Task 3
    // TODO: Test microSD Driver with physical connection
    // microSDWrite_createTask();

    return NULL;
}
