#pragma once

#include <icall.h>
#include <icall_ble_api.h>
#include <gap.h>
#include <gatt.h>
#include <gattservapp.h>
#include <gap_scanner.h>
#include <gap_initiator.h>
#include <bcomdef.h>

// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

// BLE Central Macros
#define DEFAULT_SCAN_PHY           SCAN_PRIM_PHY_1M
#define SCAN_TYPE_PASSIVE          SCAN_TYPE_PASSIVE
#define SCAN_INTERVAL              160  // 100ms
#define SCAN_WINDOW                160  // 100ms

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

void BLECentral_startScanning(void);

void scanCb(uint32_t event, void *pBuf, uintptr_t arg);

void BLECentral_connectToPeripheral(uint8_t *peerAddr, uint8_t addrType);

void BLE_readCharacteristic(uint16_t connHandle, uint16_t charHandle);
