#pragma once

// --- DEFINES --- //
// Define task parameters
#define UART_BRIDGE_TASK_STACK_SIZE   (UART_BRIDGE_STACK_SIZE)
#define UART_BRIDGE_TASK_PRIORITY     (UART_BRIDGE_PRIORITY)
Task_Struct g_uartBridgeTaskStruct;
Task_Handle g_uartBridgeTaskHandle;
uint8_t g_uartBridgeTaskStack[UART_BRIDGE_TASK_STACK_SIZE];


// --- TYPE DEFINITIONS --- //


// --- FUNCTION PROTOTYPES --- //
/**
 * @brief Constructs the UART Bridge task
 *
 * This function initializes and constructs the UART Bridge task that handles
 * wireless communication between the control box and monitor belt.
 *
 * @return Task_Handle Handle to the constructed task
 */
Task_Handle uartBridge_constructTask();

/**
 * @brief Main execution function for the UART Bridge task
 *
 * This function implements the UART Bridge task that establishes a wireless
 * connection with the monitor belt and handles sending/receiving vital sign data.
 * It automatically attempts to connect on startup and will continuously try to
 * reconnect if the connection fails.
 *
 * @param arg0 Task argument 0 (unused)
 * @param arg1 Task argument 1 (unused)
 */
void uartBridge_executeTask(UArg arg0, UArg arg1);
