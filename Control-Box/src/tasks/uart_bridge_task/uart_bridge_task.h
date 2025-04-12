#pragma once

// Define task parameters
#define UART_BRIDGE_TASK_STACK_SIZE   (UART_BRIDGE_STACK_SIZE)
#define UART_BRIDGE_TASK_PRIORITY     (UART_BRIDGE_PRIORITY)
Task_Struct g_uartBridgeTaskStruct;
Task_Handle g_uartBridgeTaskHandle;
uint8_t g_uartBridgeTaskStack[UART_BRIDGE_TASK_STACK_SIZE];

/**
 *
 */
Task_Handle uartBridge_constructTask();

/**
 *
 */
void uartBridge_executeTask(UArg arg0, UArg arg1);
