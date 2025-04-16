#pragma once

// --- DEFINES --- //
// Define task parameters
#define UART_BRIDGE_TASK_STACK_SIZE   (UART_BRIDGE_STACK_SIZE)
#define UART_BRIDGE_TASK_PRIORITY     (UART_BRIDGE_PRIORITY)
Task_Struct g_uartBridgeTaskStruct;
Task_Handle g_uartBridgeTaskHandle;
uint8_t g_uartBridgeTaskStack[UART_BRIDGE_TASK_STACK_SIZE];

/* Packet RX Configuration */
#define DATA_ENTRY_HEADER_SIZE 8  /* Constant header size of a Generic Data Entry */
#define MAX_LENGTH             64 /* Max length byte the radio will accept */
#define NUM_DATA_ENTRIES       2  /* NOTE: Only two data entries supported at the moment */
#define NUM_APPENDED_BYTES     2  /* The Data Entries data field will contain:
                                   * 1 Header byte (RF_cmdPropRx_custom2400_0.rxConf.bIncludeHdr = 0x1)
                                   * Max 30 payload bytes
                                   * 1 status byte (RF_cmdPropRx_custom2400_0.rxConf.bAppendStatus = 0x1) */
#define NO_PACKET              0
#define PACKET_RECEIVED        1


// --- TYPE DEFINITIONS --- //


// --- VARIABLE DECLARATIONS --- //
static RF_Object rfObject;
static RF_Handle rfHandle;
RF_CmdHandle rfPostHandle;

static char         input[MAX_LENGTH];
volatile uint8_t packetRxCb;
volatile size_t bytesReadCount;


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

/* Callback function called when data is received via RF
 * Function copies the data in a variable, packet, and sets packetRxCb */
void ReceivedOnRFcallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);