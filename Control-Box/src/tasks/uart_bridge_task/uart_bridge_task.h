#pragma once

// Define task parameters
#define UART_BRIDGE_TASK_STACK_SIZE   (UART_BRIDGE_STACK_SIZE)
#define UART_BRIDGE_TASK_PRIORITY     (UART_BRIDGE_PRIORITY)
Task_Struct g_uartBridgeTaskStruct;
Task_Handle g_uartBridgeTaskHandle;
uint8_t g_uartBridgeTaskStack[UART_BRIDGE_TASK_STACK_SIZE];

// MACROS
// RF UART Bridge specific definitions
#define MAX_LENGTH 32  // Minimalist approach - only need to send two values
#define DATA_ENTRY_HEADER_SIZE 8  // Constant header size of a Generic Data Entry
#define NUM_DATA_ENTRIES 2  // Only two data entries supported
#define NUM_APPENDED_BYTES 2  // Header byte + status byte

// Connection status
#define UART_BRIDGE_STATUS_DISCONNECTED 0
#define UART_BRIDGE_STATUS_CONNECTED 1
#define UART_BRIDGE_STATUS_CONNECTING 2

// TYPE DEFINITIONS
typedef struct {
    uint8_t heartRate;
    uint8_t respiratoryRate;
} VitalSigns_t;

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

/* ======== PLEASE READ THIS ========
*
* This function is declared weak for the application to override it,
* and the undefined function call below is intended to catch the
* developer's attention at link time.
* A new definition of 'rfDriverCallbackAntennaSwitching' is required.
*
* Please copy this function definition to create your own, but make
* sure to remove '__attribute__((weak))' for your definition and
* remove the below function declaration and function call.
*
* To handle the events listed in '.globalEventMask', please see the
* help text provided in 'rfDriverCallback' above.
*
* For an example on how to interact with the selected antenna pins,
* please see the code snippet below:
*
*   --- Code snippet begin ---
*
* // > This assumes antenna switch pins have been added to GPIO via
* // > sysconfig with the following properties:
* // >     mode: Output
* // >     initialOutputState: Low
* // >     outputStrength: High
*
* // > Set pin output value manually
* GPIO_write(CONFIG_RF_24GHZ, 0); // Low
* GPIO_write(CONFIG_RF_24GHZ, 1); // High
*
* // > Mux pin to be driven/controlled by the RF Core
* // > (RFC GPIO0 is here only used as an example)
* GPIO_setConfigAndMux(CONFIG_RF_24GHZ,GPIO_CFG_OUTPUT | GPIO_CFG_INPUT, IOC_PORT_RFC_GPO0);
*
* // > Mux pin to be controlled manually (i.e. release RF Core control)
* GPIO_setConfigAndMux(CONFIG_RF_24GHZ,GPIO_CFG_OUTPUT | GPIO_CFG_INPUT, IOC_PORT_GPIO);
*
*   --- Code snippet end ---
*/
void rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void *arg)

/**
 * @brief Sends vital sign data over the RF link
 *
 * This function sends heart rate and respiratory rate data to the connected device.
 *
 * @param heartRate Heart rate value to send
 * @param respiratoryRate Respiratory rate value to send
 * @return int 0 if successful, error code otherwise
 */
int uartBridge_sendVitalSigns(uint8_t heartRate, uint8_t respiratoryRate);

/**
 * @brief Initialize the RF driver
 *
 * @return int 0 if successful, error code otherwise
 */
int initRF(void);

/**
 * @brief Setup the RX queue for RF data
 *
 * @return int 0 if successful, error code otherwise
 */
int setupRXQueue(void);

/**
 * @brief Start RF in receive mode
 *
 * @return int 0 if successful, error code otherwise
 */
int startReceive(void);

/**
 * @brief Send a packet over RF
 *
 * @param data Pointer to data buffer to send
 * @param length Length of data to send
 * @return int 0 if successful, error code otherwise
 */
int sendPacket(uint8_t* data, uint8_t length);

/**
 * @brief RF event callback function
 *
 * @param h RF handle
 * @param ch Command handle
 * @param e Event mask
 */void RF_eventCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);