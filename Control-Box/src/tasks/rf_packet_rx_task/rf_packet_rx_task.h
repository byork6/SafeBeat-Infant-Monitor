#pragma once

///////        SDK HEADER CONTENT      ///////
/////// FROM RF PACKET RECEIVE EXAMPLE ///////
/* Packet RX Configuration */
#define DATA_ENTRY_HEADER_SIZE 8  /* Constant header size of a Generic Data Entry */
#define MAX_LENGTH             30 /* Max length byte the radio will accept */
#define NUM_DATA_ENTRIES       2  /* NOTE: Only two data entries supported at the moment */
#define NUM_APPENDED_BYTES     2  // The Data Entries data field will contain: 
                                  // 1 Header byte (RF_cmdPropRx.rxConf.bIncludeHdr = 0x1) 
                                  // Max 30 payload bytes 
                                  // 1 status byte (RF_cmdPropRx.rxConf.bAppendStatus = 0x1)

/***** Prototypes *****/
static void callback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

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

/////// CUSTOM HEADER CONTENT ///////
// Define task parameters
#define RF_PACKET_RX_TASK_STACK_SIZE   1024
#define RF_PACKET_RX_TASK_PRIORITY     1
Task_Struct g_RfPacketRxTaskStruct;
uint8_t g_rfPacketRxTaskStruct[RF_PACKET_RX_TASK_STACK_SIZE];

/**
* @brief  Creates an RTOS task for rf packet receive operations.
*
* This function initializes a TI-RTOS task using the Task_construct API.
* It sets up the stack, priority, and parameters required for the RTOS task execution.
* The task is statically allocated, making it suitable for real-time systems
* where dynamic memory allocation is undesirable.
*
* @return None
*/
void rfPacketRx_createTask();

/**
* @brief Executes the rf packet receive task.
*
* @param arg0 Unused arg
* @param arg1 Unused arg
*
* @return None
*/
void rfPacketRx_executeTask(UArg arg0, UArg arg1);