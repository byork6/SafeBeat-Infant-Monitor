#pragma once

#include <ti/sysbios/knl/Task.h>

// Define task parameters
#define DISPLAY_DRIVER_TASK_STACK_SIZE   (DISPLAY_DRIVER_STACK_SIZE)
#define DISPLAY_DRIVER_TASK_PRIORITY     (DISPLAY_DRIVER_PRIORITY)

extern Task_Struct g_DisplayDriverTaskStruct;
extern Task_Handle g_displayDriverTaskHandle;
extern uint8_t g_displayDriverTaskStack[DISPLAY_DRIVER_TASK_STACK_SIZE];

Task_Handle displayDriver_constructTask(void);


void displayDriver_executeTask(UArg arg0, UArg arg1);
void initDisplay(void);
void renderDisplay(int heartRate, int respirationRate);