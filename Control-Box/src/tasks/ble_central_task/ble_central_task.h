#pragma once

// BLE5Stack_Flash
#include <icall.h>
#include <icall_ble_api.h>
// #include <ble_user_config.h>
#include <gap.h>
#include <gatt.h>
#include <gapbondmgr.h>
#include <gapgattserver.h>
#include <osal_snv.h>
#include <gattservapp.h>
#include <icall_addrs.h>
#include <icall_user_config.h>
#include <icall_platform.h>
#include <icall_ble_apimsg.h>
#include <hci.h>
#include <util.h>

// NV
#include <crc.h>
#include <nvintf.h>
#include <nvocmp.h>

// Example includes
#include <hal_assert.h>

// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

// Type Definitions
// Connected device information
typedef struct
{
  uint16_t connHandle;        // Connection Handle
  uint16_t charHandle;        // Characteristic Handle
  uint8_t  addr[B_ADDR_LEN];  // Peer Device Address
  Clock_Struct *pRssiClock;   // pointer to clock struct
} connRec_t;

// BLE global variables
// Event globally used to post local events and pend on system and local events
ICall_SyncHandle syncEvent;
// Queue object used for app messages
Queue_Struct appMsg;
Queue_Handle appMsgQueue;
// List of connections
connRec_t connList[MAX_NUM_BLE_CONNS];

// BLE Central Macros
#define DEFAULT_SCAN_PHY           SCAN_PRIM_PHY_1M
// #define SCAN_TYPE_PASSIVE          SCAN_TYPE_PASSIVE
#define SCAN_TYPE_PASSIVE          SCAN_PARAM_DFLT_TYPE
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

void rfDriverCallbackAntennaSwitching(void);
