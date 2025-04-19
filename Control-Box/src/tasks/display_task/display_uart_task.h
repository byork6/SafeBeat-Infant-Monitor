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
 * @brief Constructs and starts the display UART task.
 *
 * Initializes the UART driver and launches a task that handles UART-based
 * display operations. The task itself currently sleeps, but can be extended
 * to poll a display queue.
 *
 * @return Handle to the constructed task.
 */
 Task_Handle displayUart_constructTask(void);


/**
 * @brief Task that initializes UART and optionally processes display messages.
 *
 * Opens UART2 in blocking read/write mode. Currently loops with a sleep delay,
 * but can be extended to process display messages from a queue.
 */
void displayUart_executeTask(UArg arg0, UArg arg1);

/**
 * @brief Sends a formatted string to the UART display.
 *
 * Formats the given message with display-specific framing (such as cursor
 * positioning or color control) and sends it to the UART display using
 * blocking mode.
 *
 * @param msg The null-terminated string to display.
 */
void displayUart_updateText(const char *msg);

/**
 * @brief Sends a complete Nextion command over UART.
 *
 * All Nextion commands must be terminated with three 0xFF bytes, as described
 * in the Nextion Instruction Set (section 1.1).
 *
 * @param cmd Null-terminated string containing the command to send.
 */
void sendCmd(const char* cmd);

/**
 * @brief Sends an integer value to a Nextion text component by name.
 *
 * This is typically used to update a label or numeric field in the display
 * (e.g., sending `75` to `heartrateText.txt`).
 *
 * @param objName The name of the Nextion object (must match editor object).
 * @param val The integer value to send.
 */
void sendIntToText(const char* objName, int val);

/**
 * @brief Sends a "get vaX" command to read a threshold variable from Nextion.
 *
 * This function sends a "get" command to query a variable such as "va0"
 * and stores the returned 4-byte integer result into the provided target pointer.
 *
 * The Nextion response format is: 0x71 + 4-byte value + 3x 0xFF terminators.
 *
 * @param varName The name of the Nextion variable (e.g., "va0").
 * @param target Pointer to the integer where the result should be stored.
 * @param label Label to identify the readout in the debug message.
 */
void queryThreshold(const char* varName, volatile int* target, const char* label);
