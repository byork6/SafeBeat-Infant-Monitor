#pragma once

// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

#define SERVICE_UUID      0xFFF0
#define HEART_RATE_UUID   0xFFF1
#define RESP_RATE_UUID    0xFFF2

/**
 * @brief Constructs the BLE Central task.
 *
 * Initializes the task parameters and constructs the BLE Central task.
 * This function sets up the stack, priority, and other necessary configurations
 * before creating the task.
 *
 * @return Task_Handle The handle to the constructed BLE Central task.
 */
 Task_Handle bleCentral_constructTask(void);

/**
 * @brief Executes the BLE Central task.
 *
 * This function runs the main BLE Central logic in an infinite loop. It first
 * initializes the BLE stack and then executes the central task function.
 *
 * @param arg0 Unused argument, required for compatibility.
 * @param arg1 Unused argument, required for compatibility.
 */
 void bleCentral_executeTask(UArg arg0, UArg arg1);

// Added these. TODO: write doc string.
void BLECentral_init(void);

void BLECentral_readMeasurements(void);