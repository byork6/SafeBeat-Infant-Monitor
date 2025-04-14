#pragma once

// Display UART task config
#define DISPLAY_UART_TASK_PRIORITY     (3)
#define DISPLAY_UART_TASK_STACK_SIZE   (1024)

// External task and handle declarations
extern Task_Struct g_DisplayUartTaskStruct;
extern Task_Handle g_displayUartTaskHandle;
extern uint8_t g_displayUartTaskStack[DISPLAY_UART_TASK_STACK_SIZE];

// Constructor
Task_Handle displayUart_constructTask(void);

// Optional API to call from other tasks
void displayUart_updateText(const char *msg);
