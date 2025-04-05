#pragma once

// INCLUDES
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

// nv
#include <crc.h>
#include <nvintf.h>
#include <nvocmp.h>

// Example includes
#include <hal_assert.h>
#include <osal_list.h>

// TASK DECLARATIONS
// BLE Central Task Constants
#define BLE_CENTRAL_TASK_STACK_SIZE (BLE_CENTRAL_STACK_SIZE)
#define BLE_CENTRAL_TASK_PRIORITY   (BLE_CENTRAL_PRIORITY)
Task_Struct g_BleCentralTaskStruct;
Task_Handle g_bleCentralTaskHandle;
uint8_t g_bleCentralTaskStack[BLE_CENTRAL_TASK_STACK_SIZE];

// TYPE DEFINITIONS
// Auto connect availble groups
enum{
  AUTOCONNECT_DISABLE = 0,              // Disable
  AUTOCONNECT_GROUP_A = 1,              // Group A
  AUTOCONNECT_GROUP_B = 2               // Group B
};

// Discovery states
enum{
  BLE_DISC_STATE_IDLE,                // Idle
  BLE_DISC_STATE_MTU,                 // Exchange ATT MTU size
  BLE_DISC_STATE_SVC,                 // Service discovery
  BLE_DISC_STATE_CHAR                 // Characteristic discovery
};

// App event passed from profiles.
typedef struct{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} scEvt_t;

// Scanned device information record
typedef struct{
  uint8_t addrType;         // Peer Device's Address Type
  uint8_t addr[B_ADDR_LEN]; // Peer Device Address
} scanRec_t;

// Connected device information
typedef struct{
  uint16_t connHandle;        // Connection Handle
  uint16_t charHandle;        // Characteristic Handle
  uint8_t  addr[B_ADDR_LEN];  // Peer Device Address
  Clock_Struct *pRssiClock;   // pointer to clock struct
} connRec_t;

// Container to store paring state info when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct{
  uint16_t connHandle;
  uint8_t  status;
} scPairStateData_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPasscodeCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct{
  uint8_t deviceAddr[B_ADDR_LEN];
  uint16_t connHandle;
  uint8_t uiInputs;
  uint8_t uiOutputs;
  uint32_t numComparison;
} scPasscodeData_t;

typedef struct{
	osal_list_elem elem;
	uint8_t  addr[B_ADDR_LEN];  // member's BDADDR
	uint8_t  addrType;          // member's Address Type
	uint16_t connHandle;        // member's connection handle
	uint8_t  status;            // bitwise status flag
} groupListElem_t;

// VARIABLE DECLARATIONS
// Event globally used to post local events and pend on system and local events
ICall_SyncHandle syncEvent;
// Queue object used for app messages
Queue_Struct appMsg;
Queue_Handle appMsgQueue;
// List of connections
connRec_t connList[MAX_NUM_BLE_CONNS];

// MACROS & CONSTANTS
#define DEFAULT_SCAN_PHY            SCAN_PRIM_PHY_1M
// #define SCAN_TYPE_PASSIVE          SCAN_TYPE_PASSIVE
#define SCAN_TYPE_PASSIVE           SCAN_PARAM_DFLT_TYPE
#define SCAN_INTERVAL               160  // 100ms
#define SCAN_WINDOW                 160  // 100ms
// Duplicate menu item indices.
#define SC_ITEM_NONE                TBM_ITEM_NONE
#define SC_ITEM_ALL                 TBM_ITEM_ALL
// Note: The defines should be updated accordingly if there is any change
//       in the order of the items of the menu objects the items belong to.
#define SC_ITEM_SCANPHY             TBM_ITEM(0)  // "Set Scanning PHY"
#define SC_ITEM_STARTDISC           TBM_ITEM(1)  // "Discover Devices"
#define SC_ITEM_STOPDISC            TBM_ITEM(2)  // "Stop Discovering"
#define SC_ITEM_CONNECT             TBM_ITEM(3)  // "Connect To"
#define SC_ITEM_CANCELCONN          TBM_ITEM(4)  // "Cancel Connecting"
#define SC_ITEM_SELECTCONN          TBM_ITEM(5)  // "Work With"
#define SC_ITEM_AUTOCONNECT         TBM_ITEM(6)  // "Auto Connect"

#define SC_ITEM_GATTREAD            TBM_ITEM(0)  // "GATT Read"
#define SC_ITEM_GATTWRITE           TBM_ITEM(1)  // "GATT Write"
#define SC_ITEM_STRTRSSI            TBM_ITEM(2)  // "Start RSSI Reading"
#define SC_ITEM_STOPRSSI            TBM_ITEM(3)  // "Stop RSSI Reading"
#define SC_ITEM_UPDATE              TBM_ITEM(4)  // "Connection Update"
#define SC_ITEM_CONNPHY             TBM_ITEM(5)  // "Set Connection PHY"
#define SC_ITEM_DISCONNECT          TBM_ITEM(6)  // "Disconnect"
// Application events
// #define SC_EVT_KEY_CHANGE          0x01 ---- // Removed this line because we are not using the keys from ex.
#define SC_EVT_SCAN_ENABLED         0x02
#define SC_EVT_SCAN_DISABLED        0x03
#define SC_EVT_ADV_REPORT           0x04
#define SC_EVT_SVC_DISC             0x05
#define SC_EVT_READ_RSSI            0x06
#define SC_EVT_PAIR_STATE           0x07
#define SC_EVT_PASSCODE_NEEDED      0x08
#define SC_EVT_READ_RPA             0x09
#define SC_EVT_INSUFFICIENT_MEM     0x0A
// Simple Central Task Events
#define SC_ICALL_EVT                ICALL_MSG_EVENT_ID  // Event_Id_31
#define SC_QUEUE_EVT                UTIL_QUEUE_EVENT_ID // Event_Id_30
#define SC_ALL_EVENTS                        (SC_ICALL_EVT           | \
                                              SC_QUEUE_EVT)
#define APP_EVT_EVENT_MAX           0xA
char *appEventStrings[] = {
  "APP_EVT_ZERO              ",
  "APP_EVT_KEY_CHANGE        ",
  "APP_EVT_SCAN_ENABLED      ",
  "APP_EVT_SCAN_DISABLED     ",
  "APP_EVT_ADV_REPORT        ",
  "APP_EVT_SVC_DISC          ",
  "APP_EVT_READ_RSSI         ",
  "APP_EVT_PAIR_STATE        ",
  "APP_EVT_PASSCODE_NEEDED   ",
  "APP_EVT_READ_RPA          ",
  "APP_EVT_INSUFFICIENT_MEM  ",
};
// Spin if the expression is not true
#define SIMPLECENTRAL_ASSERT(expr) if (!(expr)) SimpleCentral_spin();
// Timeout for the initiator to cancel connection if not successful
#define CONNECTION_TIMEOUT          3000
// Auto connect chosen group
#define GROUP_NAME_LENGTH           4
//Member defalult status when initalized
#define GROUP_MEMBER_INITIALIZED    0x00
//Member connected
#define GROUP_MEMBER_CONNECTED      0x01
//Default connection handle which is set when group member is created
#define GROUP_INITIALIZED_CONNECTION_HANDLE     0xFFFF
// Size of string-converted device address ("0xXXXXXXXXXXXX")
#define SC_ADDR_STR_SIZE            15

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

/*********************************************************************
* @fn      SimpleCentral_passcodeCb
*
* @brief   Passcode callback.
*
* @param   deviceAddr - pointer to device address
*
* @param   connHandle - the connection handle
*
* @param   uiInputs - pairing User Interface Inputs
*
* @param   uiOutputs - pairing User Interface Outputs
*
* @param   numComparison - numeric Comparison 20 bits
*
* @return  none
*/
void SimpleCentral_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle, uint8_t uiInputs, uint8_t uiOutputs, uint32_t numComparison);

/*********************************************************************
 * @fn      SimpleCentral_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
void SimpleCentral_pairStateCb(uint16_t connHandle, uint8_t state, uint8_t status);

/*********************************************************************
 * @fn      SimpleCentral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
status_t SimpleCentral_enqueueMsg(uint8_t event, uint8_t state, int8_t *pData);

/*********************************************************************
 * @fn      SimpleCentral_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
uint8_t SimpleCentral_processStackMsg(ICall_Hdr *pMsg);

/*********************************************************************
 * @fn      SimpleCentral_processAppMsg
 *
 * @brief   Scanner application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
void SimpleCentral_processAppMsg(scEvt_t *pMsg);

/*********************************************************************
 * @fn		SimpleCentral_autoConnect
 *
 * @brief	Check if Advertiser is part of the group according to its Adv Data
 *
 * @param   none
 *
 * @return  none
 */
void SimpleCentral_autoConnect(void);

/*********************************************************************
 * @fn      SimpleCentral_getConnAddrStr
 *
 * @brief   Return, in string form, the address of the peer associated with
 *          the connHandle.
 *
 * @return  A null-terminated string of the address.
 *          if there is no match, NULL will be returned.
 */
char* SimpleCentral_getConnAddrStr(uint16_t connHandle);

/*********************************************************************
 * @fn		SimpleCentral_isMember
 *
 * @brief	Check if Advertiser is part of the group according to its Adv Data
 *
 * @param	advData   - pointer to adv data
 *          groupNmae - group name which need to be compared with
 *          len       - length of the group name
 *
 * @return  TRUE: part of the group
 *          FALSE: not part of the group
 */

uint8_t SimpleCentral_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len);

/*********************************************************************
 * @fn      SimpleCentral_processCmdCompleteEvt
 *
 * @brief   Process an incoming OSAL HCI Command Complete Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
void SimpleCentral_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg);

/*********************************************************************
 * @fn      SimpleCentral_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  none
 */
void SimpleCentral_processGATTMsg(gattMsgEvent_t *pMsg);

/*********************************************************************
 * @fn      SimpleCentral_processGapMsg
 *
 * @brief   GAP message processing function.
 *
 * @param   pMsg - pointer to event message structure
 *
 * @return  none
 */
void SimpleCentral_processGapMsg(gapEventHdr_t *pMsg);

/*********************************************************************
 * @fn      SimpleCentral_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
void SimpleCentral_processPairState(uint8_t state, scPairStateData_t* pPairData);

/*********************************************************************
 * @fn      SimpleCentral_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
void SimpleCentral_processPasscode(scPasscodeData_t *pData);