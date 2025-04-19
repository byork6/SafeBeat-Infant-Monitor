#pragma once

// Display UART task config
#define DISPLAY_UART_TASK_PRIORITY     (3)
#define DISPLAY_UART_TASK_STACK_SIZE   (1024)

// External task and handle declarations
extern Task_Struct g_DisplayUartTaskStruct;
extern Task_Handle g_displayUartTaskHandle;
extern uint8_t g_displayUartTaskStack[DISPLAY_UART_TASK_STACK_SIZE];

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
