// --- INCLUDES --- //
#include "../../common/common.h"
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)


/* Buffer which contains all Data Entries for receiving data.
 * Pragmas are needed to make sure this buffer is 4 byte aligned (requirement from the RF Core) */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN (rxDataEntryBuffer, 4);
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  MAX_LENGTH,
                                                  NUM_APPENDED_BYTES)];
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 4
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  MAX_LENGTH,
                                                  NUM_APPENDED_BYTES)];
#elif defined(__GNUC__)
static uint8_t
rxDataEntryBuffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_DATA_ENTRIES,
                                                  MAX_LENGTH,
                                                  NUM_APPENDED_BYTES)]
                                                  __attribute__((aligned(4)));
#else
#error This compiler is not supported.
#endif

/* Receive dataQueue for RF Core to fill in data */
static dataQueue_t dataQueue;
static rfc_dataEntryGeneral_t* currentDataEntry;
static uint8_t packetLength;
static uint8_t* packetDataPointer;

static uint8_t packet[MAX_LENGTH + NUM_APPENDED_BYTES - 1]; /* The length byte is stored in a separate variable */

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
    
    printf("Entering uartBridge_executeTask()...\n");

    packetRxCb = NO_PACKET;

    RF_Params rfParams;
    RF_Params_init(&rfParams);

    if(RFQueue_defineQueue(&dataQueue, rxDataEntryBuffer, sizeof(rxDataEntryBuffer), NUM_DATA_ENTRIES, MAX_LENGTH + NUM_APPENDED_BYTES)){
        /* Failed to allocate space for all data entries */
        printf("CRITICAL FAILURE. NO MEMORY FOR RF QUEUE. SPINNING...\n");
        while(1);
    }

    /*Modifies settings to be able to do RX*/
    /* Set the Data Entity queue for received data */
    RF_cmdPropRx_custom2400_0.pQueue = &dataQueue;
    /* Discard ignored packets from Rx queue */
    RF_cmdPropRx_custom2400_0.rxConf.bAutoFlushIgnored = 1;
    /* Discard packets with CRC error from Rx queue */
    RF_cmdPropRx_custom2400_0.rxConf.bAutoFlushCrcErr = 1;
    /* Implement packet length filtering to avoid PROP_ERROR_RXBUF */
    RF_cmdPropRx_custom2400_0.maxPktLen = MAX_LENGTH;
    RF_cmdPropRx_custom2400_0.pktConf.bRepeatOk = 1;
    RF_cmdPropRx_custom2400_0.pktConf.bRepeatNok = 1;

    RF_cmdPropTx_custom2400_0.pPkt = packet;
    RF_cmdPropTx_custom2400_0.startTrigger.triggerType = TRIG_NOW;

    /* Set the max amount of bytes to read via UART */
    size_t bytesToRead = MAX_LENGTH;

    bytesReadCount = 0;

    /* Request access to the radio */
    rfHandle = RF_open(&rfObject, &RF_prop_custom2400_0, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_custom2400_0, &rfParams);

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs_custom2400_0, RF_PriorityNormal, NULL, 0);

    rfPostHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx_custom2400_0, RF_PriorityNormal, &ReceivedOnRFcallback, RF_EventRxEntryDone);

    while (1) {
        printf("UART Bridge Count: %d\n", i++);

        // --- CODE HERE IS FOR RX --- //
        if(packetRxCb){
            memcpy(input, packet, (packetLength));

            /* Reset RF RX callback flag */
            packetRxCb = NO_PACKET;

            Task_sleep(g_taskSleepDuration);

            rfPostHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx_custom2400_0, RF_PriorityNormal, &ReceivedOnRFcallback, RF_EventRxEntryDone);
        }
        else{
            Task_sleep(g_taskSleepDuration);
        }
    }
}

void ReceivedOnRFcallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e){
    if (e & RF_EventRxEntryDone){
        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry(); //loads data from entry

        /* Handle the packet data, located at &currentDataEntry->data:
         * - Length is the first byte with the current configuration
         * - Data starts from the second byte */
        packetLength      = *(uint8_t*)(&currentDataEntry->data); //gets the packet length (send over with packet)
        packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1); //data starts from 2nd byte

        memcpy(packet, packetDataPointer, (packetLength + 1));

        RFQueue_nextEntry();

        printf("\nReceived packet length of %d bytes\n", packetLength);
        if (packetLength == 2){
            uint8_t heartRate = packet[0];
            uint8_t respiratoryRate = packet[1];
            printf("Heart Rate: %d bpm, Respiratory Rate: %d bpm\n", heartRate, respiratoryRate);
            logData((int)heartRate, (int)respiratoryRate, "N/A");
        }
        else{
            printf("Unexpected packet length: %d\n", packetLength);
            for (int i = 0; i < packetLength; i++)
                printf("Byte %d: %d\n", i, packet[i]);
        }

        packetRxCb = PACKET_RECEIVED;
    }
}
