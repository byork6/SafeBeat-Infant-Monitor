#pragma once

// INCLUDES
// ble5stack_flash
#include <ti/ble5stack_flash/icall/src/icall_platform.h>
#include <ti/ble5stack_flash/icall/src/inc/icall.h>
#include <ti/ble5stack_flash/icall/src/inc/icall_user_config.h>
#include <ti/ble5stack_flash/icall/inc/icall_addrs.h>
#include <ti/ble5stack_flash/icall/inc/icall_ble_api.h>
#include <ti/ble5stack_flash/icall/inc/icall_ble_apimsg.h>
#include <ti/ble5stack_flash/inc/att.h>
#include <ti/ble5stack_flash/inc/gap.h>
#include <ti/ble5stack_flash/inc/gapbondmgr.h>
#include <ti/ble5stack_flash/inc/gatt.h>
#include <ti/ble5stack_flash/inc/gatt_uuid.h>
#include <ti/ble5stack_flash/inc/gattservapp.h>
#include <ti/ble5stack_flash/inc/gatt_profile_uuid.h>
#include <ti/ble5stack_flash/inc/hci.h>
#include <ti/ble5stack_flash/osal/src/inc/osal_snv.h>
// nv
#include <ti/common/nv/crc.h>
#include <ti/common/nv/nvintf.h>
#include <ti/common/nv/nvocmp.h>
// util
#include <ti/ble5stack_flash/common/cc26xx/util.h>

// DEFINE GUARDS
#ifdef __cplusplus
extern "C" {
#endif

// TASK DECLARATIONS AND MACROS
// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

// MACROS


// TYPE DEFINITIONS


// VARIABLE DEFINITIONS
ICall_EntityID selfEntity;

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
