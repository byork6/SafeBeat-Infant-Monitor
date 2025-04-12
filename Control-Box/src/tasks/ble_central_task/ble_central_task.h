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
// scanner and initiator
#include <ti/ble5stack_flash/inc/gap_scanner.h>
#include <ti/ble5stack_flash/inc/gap_initiator.h>

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
#define DEFAULT_ADDRESS_MODE        ADDRMODE_PUBLIC
#define DEFAULT_SCAN_PHY            SCAN_PRIM_PHY_1M
#define SCAN_TYPE_PASSIVE           SCAN_TYPE_PASSIVE
#define SCAN_INTERVAL               160  // 100ms
#define SCAN_WINDOW                 160  // 100ms
#define DEFAULT_INIT_PHY            INIT_PHY_1M

// Heart rate service UUID
#define HEARTRATE_SERVICE_UUID      0x180D
// Heart rate measurement characteristic UUID
#define HEARTRATE_MEAS_UUID         0x2A37

// Custom service UUID for respiratory rate
#define RESPIRATORY_SERVICE_UUID    0x1809  // Using Health Thermometer service UUID as placeholder
// Custom characteristic UUID for respiratory rate
#define RESPIRATORY_MEAS_UUID       0x2A1C  // Using Temperature Measurement UUID as placeholder

// Connection states
#define BLE_STATE_IDLE              0
#define BLE_STATE_SCANNING          1
#define BLE_STATE_CONNECTING        2
#define BLE_STATE_CONNECTED         3

// TYPE DEFINITIONS
typedef struct {
    uint8_t heartRate;
    uint8_t respiratoryRate;
} VitalSigns_t;

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

/**
 * @brief Initializes the BLE Central application.
 *
 * This function sets up the BLE stack, registers the application with the ICall module,
 * and initializes the GAP Central role.
 */
void bleCentral_init(void);

/**
 * @brief Starts BLE scanning.
 *
 * This function configures the scan parameters and enables continuous scanning
 * for advertising BLE peripherals.
 */
void bleCentral_startScanning(void);

/**
 * @brief Callback function for scan results.
 *
 * This function is called when a scan result is received.
 *
 * @param event The event type.
 * @param pBuf Pointer to the event data.
 * @param arg Additional arguments.
 */
void scanCallback(uint32_t event, void *pBuf, uintptr_t arg);

/**
 * @brief Connects to a BLE peripheral device.
 *
 * This function initiates a connection to the specified BLE peripheral based on
 * its address and address type.
 *
 * @param peerAddr Pointer to the peripheral's address.
 * @param addrType Address type of the peripheral (e.g., public or random).
 */
void bleCentral_connectToPeripheral(uint8_t *peerAddr, uint8_t addrType);

/**
 * @brief Reads a characteristic value from a connected peripheral.
 *
 * Sends a GATT Read Characteristic Value request to the peripheral.
 *
 * @param connHandle Connection handle of the peripheral.
 * @param charHandle Handle of the characteristic to read.
 */
void bleCentral_readCharacteristic(uint16_t connHandle, uint16_t charHandle);

/**
 * @brief Processes BLE messages.
 *
 * This function processes BLE messages received from the stack.
 *
 * @param pMsg Pointer to the message to process.
 */
void bleCentral_processGATTMsg(gattMsgEvent_t *pMsg);

/**
 * @brief Processes GAP messages.
 *
 * This function processes GAP messages received from the stack.
 *
 * @param pMsg Pointer to the message to process.
 */
void bleCentral_processGapMsg(gapEventHdr_t *pMsg);

void NVOCMP_loadApiPtrsMin(NVINTF_nvFuncts_t *pfn);

void rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void *arg);

#ifdef __cplusplus
}
#endif
