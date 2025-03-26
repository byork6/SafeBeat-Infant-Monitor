#pragma once

// UTIL
#include <util.h>

// ICALL
#include <icall.h>
#include <icall_addrs.h>
#include <icall_user_config.h>
#include <icall_platform.h>

//ICALL BLE
#include <icall_ble_apimsg.h>
#include <ble_user_config.h>

// INCLUDE
#include <gap.h>
#include <gapbondmgr.h>
#include <gapgattserver.h>
#include <gatt.h>
#include <hci.h>
#include <osal_snv.h>

// Custom
#include <gattservapp.h>
#include <gap_scanner.h>
#include <gap_initiator.h>
#include <bcomdef.h>

// BLE Central Task Constants
#define BLE_PERIPHERAL_TASK_STACK_SIZE (BLE_PERIPHERAL_STACK_SIZE)
#define BLE_PERIPHERAL_TASK_PRIORITY   (BLE_PERIPHERAL_PRIORITY)
Task_Struct g_BlePeripheralTaskStruct;
Task_Handle g_blePeripheralTaskHandle;
uint8_t g_blePeripheralTaskStack[BLE_PERIPHERAL_TASK_STACK_SIZE];

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
 Task_Handle blePeripheral_constructTask(void);

/**
 * @brief Executes the BLE Central task.
 *
 * This function runs the main BLE Central logic in an infinite loop. It first
 * initializes the BLE stack and then executes the central task function.
 *
 * @param arg0 Unused argument, required for compatibility.
 * @param arg1 Unused argument, required for compatibility.
 */
 void blePeripheral_executeTask(UArg arg0, UArg arg1);

/**
 * @brief Initializes the BLE Central application.
 *
 * This function sets up the BLE stack, registers the application with the ICall module,
 * and initializes the GAP Central role.
 */
void BLECentral_init(void);

/**
 * @brief Starts BLE scanning.
 *
 * This function configures the scan parameters and enables continuous scanning
 * for advertising BLE peripherals.
 */
void BLECentral_startScanning(void);

/**
 * @brief Callback function for handling scan events.
 *
 * This function is triggered when a scan event occurs, such as receiving an
 * advertisement report. It may be used to filter devices or initiate a connection.
 *
 * @param event Scan event type (e.g., GAP_EVT_ADV_REPORT).
 * @param ptrBuf Pointer to the scan event data.
 * @param arg  User-defined argument (unused).
 */
void scanCallback(uint32_t event, void *ptrBuf, uintptr_t arg);

/**
 * @brief Connects to a BLE peripheral device.
 *
 * This function initiates a connection to the specified BLE peripheral based on
 * its address and address type.
 *
 * @param peerAddr Pointer to the peripheral's address.
 * @param addrType Address type of the peripheral (e.g., public or random).
 */
void BLECentral_connectToPeripheral(uint8_t *peerAddr, uint8_t addrType);

/**
 * @brief Reads a characteristic value from a connected peripheral.
 *
 * Sends a GATT Read Characteristic Value request to the peripheral.
 *
 * @param connHandle Connection handle of the peripheral.
 * @param charHandle Handle of the characteristic to read.
 */
void BLE_readCharacteristic(uint16_t connHandle, uint16_t charHandle);
