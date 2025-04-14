#include "common/common.h"
/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

/* Application Header files */
#include "src/tasks/uart_bridge_task//RFQueue.h"


/***** Defines *****/

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

/*******Global variable declarations*********/
static RF_Object rfObject;
static RF_Handle rfHandle;
RF_CmdHandle rfPostHandle;

UART2_Handle uart;
UART2_Params uartParams;

static char         input[MAX_LENGTH];
int32_t             UARTwrite_semStatus;
int_fast16_t        status = UART2_STATUS_SUCCESS;
volatile uint8_t packetRxCb;
volatile size_t bytesReadCount;

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

/***** Function definitions *****/
static void ReceivedOnRFcallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
static void ReceiveonUARTcallback(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status);

void *mainThread(void *arg0){

    // Retrieve power-reset reason
    PowerCC26X2_ResetReason resetReason = PowerCC26X2_getResetReason();

    // If we are waking up from shutdown, we do something extra.
    if (resetReason == PowerCC26X2_RESET_SHUTDOWN_IO){
        // Application code must always disable the IO latches when coming out of shutdown
        PowerCC26X2_releaseLatches();
    }

    // Initialize the board with TI-Driver config & custom config if needed
    initBOARD();
    
    // Enable the power policy -- If all tasks are blocked the idleLoop will execute the power policy. 
    // If the powerbutton is pushed Power_shutdown() will be forced.
    Power_enablePolicy();
    
    // createAllResources();

    // --- TEST UART BRIDGE EXAMPLE CODE --- //
    printf("Entering example code...\n");
    packetRxCb = NO_PACKET;

    RF_Params rfParams;
    RF_Params_init(&rfParams);

    if(RFQueue_defineQueue(&dataQueue,
                                rxDataEntryBuffer,
                                sizeof(rxDataEntryBuffer),
                                NUM_DATA_ENTRIES,
                                MAX_LENGTH + NUM_APPENDED_BYTES))
    {
        /* Failed to allocate space for all data entries */
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

    // /* Initialize UART with callback read mode */
    // UART2_Params_init(&uartParams);
    // uartParams.baudRate = 115200;
    // uartParams.readMode = UART2_Mode_CALLBACK;
    // uartParams.readCallback = ReceiveonUARTcallback;
    // uartParams.readReturnMode = UART2_ReadReturnMode_PARTIAL;

    // /* Access UART */
    // uart = UART2_open(CONFIG_UART2_0, &uartParams);

    // /* Print to the terminal that the program has started */
    // const char        startMsg[] = "\r\nRF-UART bridge started:\r\n";
    // UART2_write(uart, startMsg, sizeof(startMsg), NULL);

    /* Request access to the radio */
    rfHandle = RF_open(&rfObject, &RF_prop_custom2400_0, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_custom2400_0, &rfParams);

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs_custom2400_0, RF_PriorityNormal, NULL, 0);

    rfPostHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx_custom2400_0,
                                                           RF_PriorityNormal, &ReceivedOnRFcallback,
                                                           RF_EventRxEntryDone);

    // UART2_read(uart, &input, bytesToRead, NULL);

    while(1)
    {
        printf("In while loop\n");
        /* Check if anything has been received via RF*/
        if(packetRxCb)
        {
            memcpy(input, packet, (packetLength));
            printf("Packet Received via RF\n");
            size_t bytesWritten = 0;
            while (bytesWritten == 0)
            {
                status = UART2_write(uart, &input, packetLength, &bytesWritten);
                if (status != UART2_STATUS_SUCCESS)
                {
                    /* UART2_write() failed */
                    while (1);
                }
            }

            /* Reset RF RX callback flag */
            packetRxCb = NO_PACKET;
        }

        /* Check if anything has been received via UART*/
        // --- CODE HERE IS FOR TX DATA --- //
        static bool sent = false;
        if (!sent)
        {
            printf("Sending data over RF...\n");
            // Example: Send two local values (e.g., HR and RR)
            uint8_t heartRate = 75;
            uint8_t respRate  = 18;

            packet[0] = heartRate;
            packet[1] = respRate;

            RF_cmdPropTx_custom2400_0.pktLen = 2; // 2 bytes

            // Cancel RX command
            RF_cancelCmd(rfHandle, rfPostHandle, 1);

            // Send packet over RF
            RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx_custom2400_0, RF_PriorityNormal, NULL, 0);

            // Resume RX
            rfPostHandle = RF_postCmd(rfHandle, (RF_Op*)&RF_cmdPropRx_custom2400_0,
                                        RF_PriorityNormal, &ReceivedOnRFcallback,
                                        RF_EventRxEntryDone);

            sent = true;
        }
    }


    return NULL;
}

/* Callback function called when data is received via RF
 * Function copies the data in a variable, packet, and sets packetRxCb */
void ReceivedOnRFcallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if (e & RF_EventRxEntryDone)
    {
        // GPIO_toggle(CONFIG_GPIO_RLED);

        /* Get current unhandled data entry */
        currentDataEntry = RFQueue_getDataEntry(); //loads data from entry

        /* Handle the packet data, located at &currentDataEntry->data:
         * - Length is the first byte with the current configuration
         * - Data starts from the second byte */
        packetLength      = *(uint8_t*)(&currentDataEntry->data); //gets the packet length (send over with packet)
        packetDataPointer = (uint8_t*)(&currentDataEntry->data + 1); //data starts from 2nd byte

        /* Copy the payload + the status byte to the packet variable */
        memcpy(packet, packetDataPointer, (packetLength + 1));

        /* Move read entry pointer to next entry */
        RFQueue_nextEntry();

        /* Print packet data (optional) */
        printf("RF Received packet of length %d: ", packetLength);
        for (int i = 0; i < packetLength; i++)
        {
            printf("0x%02X ", packet[i]);
        }
        printf("\n");

        packetRxCb = PACKET_RECEIVED;
    }
}

/* Callback function called when data is received via UART */
void ReceiveonUARTcallback(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status)
{
    if (status != UART2_STATUS_SUCCESS)
    {
        /* RX error occured in UART2_read() */
        while (1) {}
    }

    bytesReadCount = count;
}

