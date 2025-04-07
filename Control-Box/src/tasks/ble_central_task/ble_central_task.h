#pragma once

// INCLUDES
// BLE5Stack
#include <ti/ble5stack/inc/gap.h>
#include <ti/ble5stack/inc/gatt.h>
#include <ti/ble5stack/inc/gatt_uuid.h>
#include <ti/ble5stack/inc/gatt_profile_uuid.h>
#include <ti/ble5stack/inc/gattservapp.h>
#include <ti/ble5stack/inc/att.h>
#include <ti/ble5stack/icall/src/inc/icall.h>
#include <ti/ble5stack/icall/inc/icall_ble_api.h>
#include <ti/ble5stack/inc/gapbondmgr.h>

// DEFINE GUARDS
#ifdef __cplusplus
extern "C" {
#endif

// TASK DECLARATIONS
// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

// TYPE DEFINITIONS

// MACROS & CONSTANTS

// FUNCTION PROTOTYPES
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


void bleCentral_init(void);
