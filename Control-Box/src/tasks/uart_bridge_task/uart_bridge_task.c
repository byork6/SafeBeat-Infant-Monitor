#include "../../common/common.h"

// TYPE DEFINITIONS


// STATIC DECLARATIONS
// RF Driver handle
static RF_Object rfObject;
static RF_Handle rfHandle;

// RF command handles
static RF_CmdHandle rxCmdHandle;
static RF_CmdHandle txCmdHandle;

// Receive dataQueue for RF Core to fill in data
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t packetLength;
static uint8_t* packetDataPointer;

// Packet buffers
static uint8_t rxPacket[MAX_LENGTH + NUM_APPENDED_BYTES - 1];
static uint8_t txPacket[MAX_LENGTH];

// VARIABLE DEFINITIONS
// Connection status
static volatile uint8_t connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;

// Vital signs data
static VitalSigns_t vitalSigns = {0, 0};

// Buffer which contains all Data Entries for receiving data
// Aligned to 4 bytes (requirement from the RF Core)
#pragma DATA_ALIGN (rxDataEntryBuffer, 4);
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  MAX_LENGTH,
                                                  NUM_APPENDED_BYTES)];


Task_Handle uartBridge_constructTask() {
    // Declare TaskParams struct name
    Task_Params TaskParams;

    // Initialize TaskParams and set parameters
    Task_Params_init(&TaskParams);
    // Stack array
    TaskParams.stack = g_uartBridgeTaskStack;
    // Stack array size
    TaskParams.stackSize = UART_BRIDGE_TASK_STACK_SIZE;
    // Stack task TI-RTOS priority
    TaskParams.priority = UART_BRIDGE_TASK_PRIORITY;
    // arg0 and ar1 passed to execution function for the created task. Use 0 if arg is unused.
    // you can pass variables or pointers to structs for larger data objects.
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    // Construct the TI-RTOS task using the API
    Task_construct(&g_uartBridgeTaskStruct, uartBridge_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_uartBridgeTaskStruct;
}

void uartBridge_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;
    int i = 0;
    int status;
    uint8_t reconnectAttempts = 0;

    printf("Entering uartBridge_executeTask()...\n");

    // Initialize RF driver
    status = initRF();
    if (status != 0) {
        printf("Failed to initialize RF driver. Error code: %d\n", status);
        return;
    }

    // Setup RX queue for RF
    status = setupRXQueue();
    if (status != 0) {
        printf("Failed to setup RX queue. Error code: %d\n", status);
        return;
    }

    printf("UART Bridge initialized. Attempting to connect...\n");
    connectionStatus = UART_BRIDGE_STATUS_CONNECTING;

    // Start receiving - this will also attempt to establish connection
    status = startReceive();
    if (status != 0) {
        printf("Failed to start RF receive. Error code: %d\n", status);
        connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;
    }
    printf("Recieve mode started.\n");

    while (1) {
        printf("UART Bridge Count: %d\n", i++);
        printf("UART connection status: %d\n", connectionStatus);
        // Check connection status
        if (connectionStatus == UART_BRIDGE_STATUS_DISCONNECTED) {
            // Try to reconnect
            reconnectAttempts++;
            printf("STATUS: NOT CONNECTED\nAttempting to reconnect...\n");

            connectionStatus = UART_BRIDGE_STATUS_CONNECTING;
            status = startReceive();

            if (status != 0) {
                printf("Reconnect failed. Total failed reconnection attempts: %d\nWill retry...\n", reconnectAttempts);
                connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;
            }
            else{
                printf("Reconnection successful.\n");
                reconnectAttempts = 0;
            }
        }
        else if (connectionStatus == UART_BRIDGE_STATUS_CONNECTED) {
            printf("STATUS: CONNECTED\nSending data to Monitor-Belt...\n");
            // Reset reconnect counter when connected
            reconnectAttempts = 0;

            // In a real implementation, you would get heart rate and respiratory rate
            // from sensors or other tasks and send them periodically
            // For demonstration, we'll just use dummy values
            static uint8_t dummyHeartRate = 70;
            static uint8_t dummyRespRate = 16;

            // Send vital signs every few seconds
            status = uartBridge_sendVitalSigns(dummyHeartRate, dummyRespRate);

            // Update dummy values for next iteration (in real app, these would come from sensors)
            dummyHeartRate = (dummyHeartRate >= 100) ? 70 : dummyHeartRate + 1;
            dummyRespRate = (dummyRespRate >= 25) ? 16 : dummyRespRate + 1;
        }

        // Sleep to allow other tasks to run
        Task_sleep(g_taskSleepDuration);
    }
}

void rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void *arg){
    // GPIO_write(CONFIG_RF_24GHZ, 0); // Low
    // GPIO_write(CONFIG_RF_24GHZ, 1); // High
    // GPIO_setConfigAndMux(CONFIG_RF_24GHZ,GPIO_CFG_OUTPUT | GPIO_CFG_INPUT, IOC_PORT_RFC_GPO0);
    // GPIO_setConfigAndMux(CONFIG_RF_24GHZ,GPIO_CFG_OUTPUT | GPIO_CFG_INPUT, IOC_PORT_GPIO);
    /* ======== PLEASE READ THIS ========
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
    // extern void you_must_implement_rfDriverCallbackAntennaSwitching_see_generated_ti_drivers_config_c_in_debug_folder_or_sysconfig_preview(void);
    // you_must_implement_rfDriverCallbackAntennaSwitching_see_generated_ti_drivers_config_c_in_debug_folder_or_sysconfig_preview();
}

int uartBridge_sendVitalSigns(uint8_t heartRate, uint8_t respiratoryRate) {
    // Prepare packet with vital signs data
    txPacket[0] = 0x01;  // Packet type: vital signs
    txPacket[1] = heartRate;
    txPacket[2] = respiratoryRate;

    // Send the packet
    return sendPacket(txPacket, 3);
}

int initRF(void) {
    RF_Params rfParams;

    // Initialize RF parameters
    RF_Params_init(&rfParams);

    // Open RF driver
    rfHandle = RF_open(&rfObject, &RF_prop_custom2400_0, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_custom2400_0, &rfParams);
    if (rfHandle == NULL) {
        return -1;
    }

    // Set the frequency
    RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs_custom2400_0, RF_PriorityNormal, NULL, 0);

    return 0;
}

int setupRXQueue(void) {
    if (RFQueue_defineQueue(&dataQueue,
                          rxDataEntryBuffer,
                          sizeof(rxDataEntryBuffer),
                          NUM_DATA_ENTRIES,
                          MAX_LENGTH + NUM_APPENDED_BYTES)) {
        return -1;
    }

    // Set the Data Entity queue for received data
    RF_cmdPropRx_custom2400_0.pQueue = &dataQueue;
    // Discard ignored packets from Rx queue
    RF_cmdPropRx_custom2400_0.rxConf.bAutoFlushIgnored = 1;
    // Discard packets with CRC error from Rx queue
    RF_cmdPropRx_custom2400_0.rxConf.bAutoFlushCrcErr = 1;
    // Implement packet length filtering to avoid PROP_ERROR_RXBUF
    RF_cmdPropRx_custom2400_0.maxPktLen = MAX_LENGTH;
    // Continuous operation
    RF_cmdPropRx_custom2400_0.pktConf.bRepeatOk = 1;
    RF_cmdPropRx_custom2400_0.pktConf.bRepeatNok = 1;

    return 0;
}

int startReceive(void) {
    // Cancel any ongoing RF operations
    RF_cancelCmd(rfHandle, rxCmdHandle, 0);

    // Post RX command to start receiving RF data
    rxCmdHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx_custom2400_0,
                           RF_PriorityNormal, &RF_eventCallback,
                           RF_EventRxEntryDone | RF_EventRxOk | RF_EventRxNOk);

    if (rxCmdHandle < 0) {
        return -1;
    }

    return 0;
}

int sendPacket(uint8_t* data, uint8_t length) {
    // Ensure we're not trying to send too much data
    if (length > MAX_LENGTH) {
        return -1;
    }

    // Cancel ongoing RX
    RF_cancelCmd(rfHandle, rxCmdHandle, 0);

    // Configure TX command
    RF_cmdPropTx_custom2400_0.pktLen = length;
    RF_cmdPropTx_custom2400_0.pPkt = data;
    RF_cmdPropTx_custom2400_0.startTrigger.triggerType = TRIG_NOW;

    // Send packet
    txCmdHandle = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx_custom2400_0, RF_PriorityNormal, NULL, 0);

    if (txCmdHandle < 0) {
        // Restart RX and return error
        startReceive();
        return -1;
    }

    // Toggle LED to indicate TX (if available)
    // GPIO_toggle(CONFIG_GPIO_GLED);

    // Restart RX after TX
    startReceive();

    return 0;
}

void RF_eventCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e) {
    if (e & RF_EventRxEntryDone) {
        // Toggle LED to indicate RX (if available)
        // GPIO_toggle(CONFIG_GPIO_RLED);

        // Get current unhandled data entry
        currentDataEntry = RFQueue_getDataEntry();

        // Handle the packet data
        packetLength = *(uint8_t*)(&currentDataEntry->data);
        packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1);

        // Copy the payload + the status byte to the packet variable
        memcpy(rxPacket, packetDataPointer, (packetLength + 1));

        // Process received packet
        if (packetLength > 0) {
            // Update connection status
            connectionStatus = UART_BRIDGE_STATUS_CONNECTED;

            // Process packet based on type
            if (rxPacket[0] == 0x01 && packetLength >= 3) {
                // Vital signs packet
                vitalSigns.heartRate = rxPacket[1];
                vitalSigns.respiratoryRate = rxPacket[2];

                printf("Received vital signs - HR: %d, RR: %d\n",
                       vitalSigns.heartRate, vitalSigns.respiratoryRate);
            }
        }

        // Move read entry pointer to next entry
        RFQueue_nextEntry();
    }
    else if (e & RF_EventRxNOk) {
        // RX error occurred
        printf("RF RX error\n");
    }
}
