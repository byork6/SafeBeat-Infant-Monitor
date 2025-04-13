#include "../../common/common.h"


// --- TYPE DEFINITIONS --- //


// --- VARIABLE DECLARATIONS --- //
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


// --- VARIABLE DEFINITIONS --- //
// Connection status
static volatile uint8_t connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;

// Vital signs data
static VitalSigns_t vitalSigns = {0, 0};

// Buffer which contains all Data Entries for receiving data
// Aligned to 4 bytes (requirement from the RF Core)
static uint8_t rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES, MAX_LENGTH, NUM_APPENDED_BYTES)] __attribute__((aligned(4)));

static const char* connectionStatusStrings[] = {
    "DISCONNECTED", // 0
    "CONNECTED",    // 1
    "CONNECTING"    // 2
};


// --- FUNCTION DEFINITIONS --- //
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
    int rfStatus = 0;
    int rxQueueStatus = 0;
    int receiveStatus = 0;
    int transmissionStatus = 0;
    uint8_t reconnectionAttempts = 0;
    
    printf("Entering uartBridge_executeTask()...\n");

    // Initialize RF driver
    rfStatus = initRF();
    if (rfStatus != 0) {
        printf("Failed to initialize RF driver. Error code: %d\n", rfStatus);
        return;
    }
    printf("RF Driver initialized.\n");

    // Setup RX queue for RF
    rxQueueStatus = setupRXQueue();
    if (rxQueueStatus != 0) {
        printf("Failed to setup RX queue. Error code: %d\n", rxQueueStatus);
        return;
    }
    printf("RX Queue initialized.\n");

    connectionStatus = UART_BRIDGE_STATUS_CONNECTING;

    // Start receiving - this will also attempt to establish connection
    receiveStatus = startReceive();
    if (receiveStatus != 0) {
        printf("Failed to start RF receive. Error code: %d\n", receiveStatus);
        connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;
    }
    printf("Recieve mode started. Attempting to connect...\n");

    while (1) {
        printf("UART Bridge Count: %d\n", i++);
        printf("UART connection status: %s\n", connectionStatusStrings[connectionStatus]);
        // Check connection status
        if (connectionStatus == UART_BRIDGE_STATUS_DISCONNECTED) {
            // Try to reconnect
            reconnectionAttempts++;
            connectionStatus = UART_BRIDGE_STATUS_CONNECTING;

            receiveStatus = startReceive();
            if (receiveStatus != 0) {
                printf("Reconnect failed. Total failed reconnection attempts: %d\nWill retry...\n", reconnectionAttempts);
                connectionStatus = UART_BRIDGE_STATUS_DISCONNECTED;
            }
            printf("Recieve mode started. Attempting to connect...\n");
        }
        else if (connectionStatus == UART_BRIDGE_STATUS_CONNECTED) {           
            printf("Waiting to recieve data from active connection...\n");
        }

        // Sleep to allow other tasks to run
        Task_sleep(g_taskSleepDuration);
    }
}

int initRF(void) {
    RF_Params rfParams;
    RF_EventMask rfEvent;

    // Initialize RF parameters
    RF_Params_init(&rfParams);

    // Open RF driver
    rfHandle = RF_open(&rfObject, &RF_prop_custom2400_0, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_custom2400_0, &rfParams);
    if (rfHandle == NULL) {
        return -1;
    }

    // Set the frequency
    rfEvent = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs_custom2400_0, RF_PriorityNormal, NULL, 0);

    // NOTE: Event list shown in RFCC26X2.h on line 730
    printf("RF_runCmd event occurred");
    if (rfEvent & RF_EventCmdDone)             printf(" --- RF_EventCmdDone\n");
    if (rfEvent & RF_EventLastCmdDone)         printf(" --- RF_EventLastCmdDone\n");
    if (rfEvent & RF_EventFGCmdDone)           printf(" --- RF_EventFGCmdDone\n");
    if (rfEvent & RF_EventLastFGCmdDone)       printf(" --- RF_EventLastFGCmdDone\n");
    if (rfEvent & RF_EventTxDone)              printf(" --- RF_EventTxDone\n");
    if (rfEvent & RF_EventTXAck)               printf(" --- RF_EventTXAck\n");
    if (rfEvent & RF_EventTxCtrl)              printf(" --- RF_EventTxCtrl\n");
    if (rfEvent & RF_EventTxCtrlAck)           printf(" --- RF_EventTxCtrlAck\n");
    if (rfEvent & RF_EventTxRetrans)           printf(" --- RF_EventTxRetrans\n");
    if (rfEvent & RF_EventTxEntryDone)         printf(" --- RF_EventTxEntryDone\n");
    if (rfEvent & RF_EventTxBufferChange)      printf(" --- RF_EventTxBufferChange\n");
    if (rfEvent & RF_EventPaChanged)           printf(" --- RF_EventPaChanged\n");
    if (rfEvent & RF_EventSamplesEntryDone)    printf(" --- RF_EventSamplesEntryDone\n");
    if (rfEvent & RF_EventRxOk)                printf(" --- RF_EventRxOk\n");
    if (rfEvent & RF_EventRxIgnored)           printf(" --- RF_EventRxIgnored\n");
    if (rfEvent & RF_EventRxEmpty)             printf(" --- RF_EventRxEmpty\n");
    if (rfEvent & RF_EventRxCtrl)              printf(" --- RF_EventRxCtrl\n");
    if (rfEvent & RF_EventRxCtrlAck)           printf(" --- RF_EventRxCtrlAck\n");
    if (rfEvent & RF_EventRxEntryDone)         printf(" --- RF_EventRxEntryDone\n");
    if (rfEvent & RF_EventDataWritten)         printf(" --- RF_EventDataWritten\n");
    if (rfEvent & RF_EventNDataWritten)        printf(" --- RF_EventNDataWritten\n");
    if (rfEvent & RF_EventRxAborted)           printf(" --- RF_EventRxAborted\n");
    if (rfEvent & RF_EventRxCollisionDetected) printf(" --- RF_EventRxCollisionDetected\n");
    if (rfEvent & RF_EventModulesUnlocked)     printf(" --- RF_EventModulesUnlocked\n");
    if (rfEvent & RF_EventInternalError)       printf(" --- RF_EventInternalError\n");
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
                
                logData(vitalSigns.heartRate, vitalSigns.respiratoryRate, "N/A");
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

// FOR TX
int uartBridge_sendVitalSigns(uint8_t heartRate, uint8_t respiratoryRate) {
    // Prepare packet with vital signs data
    txPacket[0] = 0x01;  // Packet type: vital signs
    txPacket[1] = heartRate;
    txPacket[2] = respiratoryRate;

    // Send the packet
    return sendPacket(txPacket, 3);
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