#pragma once

#include <ti/drivers/UART2.h>
#include <ti/sysbios/knl/Task.h>

#define DISPLAY_UART_TASK_STACK_SIZE  (DISPLAY_DRIVER_STACK_SIZE)
#define DISPLAY_UART_TASK_PRIORITY    (DISPLAY_DRIVER_PRIORITY)

// Global handles
Task_Struct g_DisplayUartTaskStruct;
Task_Handle g_displayUartTaskHandle;
uint8_t g_displayUartTaskStack[DISPLAY_UART_TASK_STACK_SIZE];

/**
 * @brief Constructs the Display UART task with static allocation.
 *
 * Initializes UART2 with 115200 baud rate, and creates a task to send
 * updated HR/RR values to the Nextion display, and read updated thresholds.
 *
 * @return Handle to the created Display UART task
 */
Task_Handle displayUart_constructTask();

/**
 * @brief Executes the main UART communication loop with the display.
 *
 * Pushes data to screen every 2 seconds, and queries thresholds.
 */
void displayUart_executeTask(UArg arg0, UArg arg1);
