#pragma once

// Define task parameters
#define DISPLAY_DRIVER_TASK_STACK_SIZE   (DISPLAY_DRIVER_STACK_SIZE)
#define DISPLAY_DRIVER_TASK_PRIORITY     (DISPLAY_DRIVER_PRIORITY)

extern Task_Struct g_DisplayDriverTaskStruct;
extern Task_Handle g_displayDriverTaskHandle;
extern uint8_t g_displayDriverTaskStack[DISPLAY_DRIVER_TASK_STACK_SIZE];

//TODO: Add doc strings for all the function prototypes below, chat can generate these, use the template from common.h, show chat the template and tell it write the strings
// for all of these and you can copy them in.

Task_Handle displayDriver_constructTask(void);

void displayDriver_executeTask(UArg arg0, UArg arg1);

void renderDisplay(int heartRate, int respirationRate);

void initializeDrivers(void);
