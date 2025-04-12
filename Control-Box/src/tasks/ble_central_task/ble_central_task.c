#include "common.h"

// VARIABLE DECLARATIONS
// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;
// Address Mode
static uint8_t addrMode = DEFAULT_ADDRESS_MODE;
// Random address - not used with public address mode
// uint8_t pRandomAddress[B_ADDR_LEN] = {0};

// Scan parameters
static uint16_t scanResFields = GAP_ADTYPE_FLAGS | GAP_ADTYPE_16BIT_MORE | GAP_ADTYPE_LOCAL_NAME_COMPLETE;
// Connection handle
static uint16_t connHandle = LINKDB_CONNHANDLE_INVALID;
// Connection state
static uint8_t connectionState = BLE_STATE_IDLE;
// Target device address
static uint8_t targetDeviceAddr[B_ADDR_LEN] = {0};
static uint8_t targetDeviceAddrType = ADDRMODE_PUBLIC;
// Service discovery state
static bool serviceDiscoveryComplete = false;
// Characteristic handles
static uint16_t heartRateCharHandle = 0;
static uint16_t respiratoryRateCharHandle = 0;
// Vital signs data
static VitalSigns_t vitalSigns = {0};
// Peripheral device name to connect to
static const char* TARGET_DEVICE_NAME = "Monitor-Belt";
// Track the last characteristic we requested to read
static uint16_t lastRequestedChar = 0;

// FUNCTION DEFINITIONS
Task_Handle bleCentral_constructTask(void) {
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_bleCentralTaskStack;
    TaskParams.stackSize = BLE_CENTRAL_TASK_STACK_SIZE;
    TaskParams.priority = BLE_CENTRAL_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_BleCentralTaskStruct, bleCentral_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_BleCentralTaskStruct;
}

void bleCentral_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    ICall_Errno errno;
    ICall_ServiceEnum src;
    ICall_EntityID dest;
    ICall_Hdr *pMsg = NULL;

    printf("Initializing BLE Central...\n");
    bleCentral_init();

    // Start scanning for peripheral devices
    printf("Starting BLE scanning...\n");
    bleCentral_startScanning();
    connectionState = BLE_STATE_SCANNING;

    printf("Entering bleCentral_executeTask() main loop...\n");
    while (1) {
        // Wait for a message from the stack
        errno = ICall_wait(ICALL_TIMEOUT_FOREVER);

        if (errno == ICALL_ERRNO_SUCCESS) {
            // Check if we got a message
            if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS) {
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity)) {
                    // Process BLE message
                    switch (((ICall_Hdr *)pMsg)->event) {
                        case GAP_MSG_EVENT:
                            bleCentral_processGapMsg((gapEventHdr_t *)pMsg);
                            break;

                        case GATT_MSG_EVENT:
                            bleCentral_processGATTMsg((gattMsgEvent_t *)pMsg);
                            break;

                        default:
                            // Unknown event
                            break;
                    }
                }

                // Free the message
                if (pMsg) {
                    ICall_freeMsg(pMsg);
                }
            }
        }

        // If connected, periodically read the heart rate and respiratory rate
        if (connectionState == BLE_STATE_CONNECTED && serviceDiscoveryComplete) {
            static uint32_t lastReadTime = 0;
            uint32_t currentTime = Clock_getTicks();

            // Read characteristics every 1 second
            if ((currentTime - lastReadTime) >= SECONDS_TO_TICKS(1)) {
                if (heartRateCharHandle != 0) {
                    bleCentral_readCharacteristic(connHandle, heartRateCharHandle);
                }

                if (respiratoryRateCharHandle != 0) {
                    bleCentral_readCharacteristic(connHandle, respiratoryRateCharHandle);
                }

                lastReadTime = currentTime;
            }
        }

        // Short sleep to prevent CPU hogging
        Task_sleep(10);
    }
}

void bleCentral_init(void) {
    // Initialize ICall module
    printf("ICall_init\n");
    ICall_init();
    printf("ICall_createRemoteTasks\n");
    ICall_createRemoteTasks(); // Creates stack and service task

    // Register with stack to receive GAP and GATT messages
    printf("ICall_registerApp\n");
    ICall_registerApp(&selfEntity, NULL);

    // Initialize GATT
    printf("GATT_InitClient\n");
    GATT_InitClient();
    printf("GATT_RegisterForMsgs\n");
    GATT_RegisterForMsgs(selfEntity);

    // Set the GAP Role (this sets the device as Central)
    printf("GAP_DeviceInit\n");
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, pRandomAddress);
}

void bleCentral_startScanning(void) {
    // Configure scan parameters
    GapScan_setPhyParams(DEFAULT_SCAN_PHY, SCAN_TYPE_PASSIVE, SCAN_INTERVAL, SCAN_WINDOW);

    // Set scan fields to report
    GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &scanResFields);

    // Register scan callback
    GapScan_registerCb(scanCallback, NULL);

    // Start continuous scanning
    GapScan_enable(0, 0, 0);  // 0=continuous scan, 0 timeout, 0 numAdvReports
}

void scanCallback(uint32_t event, void *pBuf, uintptr_t arg) {
    if (event == GAP_EVT_ADV_REPORT) {
        GapScan_Evt_AdvRpt_t *advRpt = (GapScan_Evt_AdvRpt_t *)pBuf;

        // Check if this is the device we're looking for
        uint8_t nameFound = false;
        uint8_t nameLength = 0;
        uint8_t *pDeviceName = NULL;

        // Search for the device name in the advertising data
        uint8_t *pData = advRpt->pData;
        uint8_t remainingLength = advRpt->dataLen;

        while (remainingLength > 0) {
            uint8_t length = pData[0];
            uint8_t type = pData[1];

            // Check if this is the Complete Local Name
            if (type == GAP_ADTYPE_LOCAL_NAME_COMPLETE && length > 1) {
                pDeviceName = &pData[2];
                nameLength = length - 1;
                nameFound = true;
                break;
            }

            // Move to the next AD field
            remainingLength -= (length + 1);
            pData += (length + 1);
        }

        // If we found a name, check if it matches our target
        if (nameFound && nameLength > 0) {
            // Create a null-terminated string for comparison
            char deviceName[31] = {0}; // Max BLE name length is 30 bytes
            memcpy(deviceName, pDeviceName, (nameLength > 30) ? 30 : nameLength);

            printf("Found device: %s, RSSI: %d\n", deviceName, advRpt->rssi);

            // Check if this is our target device
            if (strcmp(deviceName, TARGET_DEVICE_NAME) == 0) {
                printf("Found target device: %s, connecting...\n", deviceName);

                // Stop scanning
                GapScan_disable();

                // Save the device address
                memcpy(targetDeviceAddr, advRpt->addr, B_ADDR_LEN);
                targetDeviceAddrType = advRpt->addrType;

                // Connect to the device
                connectionState = BLE_STATE_CONNECTING;
                bleCentral_connectToPeripheral(targetDeviceAddr, targetDeviceAddrType);
            }
        }

        // Free the report
        ICall_free(pBuf);
    }
}

void bleCentral_connectToPeripheral(uint8_t *peerAddr, uint8_t addrType) {
    printf("Connecting to device...\n");
    GapInit_connect(peerAddr, addrType, DEFAULT_INIT_PHY, 0);
}

void bleCentral_readCharacteristic(uint16_t connHandle, uint16_t charHandle) {
    // Store the characteristic handle we're reading for use in the response handler
    lastRequestedChar = charHandle;

    attReadReq_t req;
    req.handle = charHandle;

    GATT_ReadCharValue(connHandle, &req, selfEntity);
}

void bleCentral_processGATTMsg(gattMsgEvent_t *pMsg) {
    // We use the global lastRequestedChar variable to track which characteristic we're reading

    switch (pMsg->method) {
        case ATT_READ_RSP:
            // Process read response
            // Note: attReadRsp_t doesn't have a handle member, so we use our lastRequestedChar
            if (lastRequestedChar == heartRateCharHandle) {
                // Heart rate data received
                if (pMsg->msg.readRsp.pValue != NULL && pMsg->msg.readRsp.len > 0) {
                    vitalSigns.heartRate = pMsg->msg.readRsp.pValue[0];
                    printf("Heart Rate: %d bpm\n", vitalSigns.heartRate);
                }
            } else if (lastRequestedChar == respiratoryRateCharHandle) {
                // Respiratory rate data received
                if (pMsg->msg.readRsp.pValue != NULL && pMsg->msg.readRsp.len > 0) {
                    vitalSigns.respiratoryRate = pMsg->msg.readRsp.pValue[0];
                    printf("Respiratory Rate: %d breaths/min\n", vitalSigns.respiratoryRate);
                }
            }
            break;

        case ATT_FIND_BY_TYPE_VALUE_RSP:
            // Service discovery response
            // Process to find service handles
            break;

        case ATT_READ_BY_TYPE_RSP:
            // Characteristic discovery response
            // Process to find characteristic handles
            break;

        default:
            break;
    }

    // Free GATT message
    GATT_bm_free(&pMsg->msg, pMsg->method);
}

void bleCentral_processGapMsg(gapEventHdr_t *pMsg) {
    switch (pMsg->opcode) {
        case GAP_DEVICE_INIT_DONE_EVENT: {
            // GAP initialization complete
            gapDeviceInitDoneEvent_t *pEvent = (gapDeviceInitDoneEvent_t *)pMsg;

            if (pEvent->hdr.status == SUCCESS) {
                printf("GAP initialization done, starting scanning...\n");
                // Start scanning for devices
                bleCentral_startScanning();
            }
            break;
        }

        case GAP_LINK_ESTABLISHED_EVENT: {
            // Connection established
            gapEstLinkReqEvent_t *pEvent = (gapEstLinkReqEvent_t *)pMsg;

            if (pEvent->hdr.status == SUCCESS) {
                connHandle = pEvent->connectionHandle;
                connectionState = BLE_STATE_CONNECTED;
                printf("Connection established, handle: %d\n", connHandle);

                // Start service discovery
                // For simplicity, we'll just use the known characteristic handles
                // In a real implementation, you would perform service discovery here

                // Simulate service discovery completion
                serviceDiscoveryComplete = true;
                heartRateCharHandle = 0x0025;  // Example handle, replace with actual discovery
                respiratoryRateCharHandle = 0x0028;  // Example handle, replace with actual discovery

                printf("Service discovery complete\n");
                printf("Heart Rate Char Handle: 0x%04X\n", heartRateCharHandle);
                printf("Respiratory Rate Char Handle: 0x%04X\n", respiratoryRateCharHandle);
            } else {
                printf("Connection failed, status: %d\n", pEvent->hdr.status);
                // Restart scanning
                connectionState = BLE_STATE_SCANNING;
                bleCentral_startScanning();
            }
            break;
        }

        case GAP_LINK_TERMINATED_EVENT: {
            // Connection terminated
            gapTerminateLinkEvent_t *pEvent = (gapTerminateLinkEvent_t *)pMsg;

            printf("Connection terminated, reason: %d\n", pEvent->reason);
            connHandle = LINKDB_CONNHANDLE_INVALID;
            connectionState = BLE_STATE_IDLE;
            serviceDiscoveryComplete = false;

            // Restart scanning
            connectionState = BLE_STATE_SCANNING;
            bleCentral_startScanning();
            break;
        }

        default:
            break;
    }
}

void NVOCMP_loadApiPtrsMin(NVINTF_nvFuncts_t *pfn)
{
//     // Load caller's structure with pointers to the NV API functions
//     pfn->initNV       = &NVOCMP_initNvApi;
//     pfn->compactNV    = &NVOCMP_compactNvApi;
//     pfn->createItem   = NULL;
//     pfn->updateItem   = NULL;
//     pfn->deleteItem   = NULL;
//     pfn->readItem     = &NVOCMP_readItemApi;
//     pfn->readContItem = NULL;
//     pfn->writeItem    = &NVOCMP_writeItemApi;
//     pfn->getItemLen   = NULL;
//     pfn->lockNV       = NULL;
//     pfn->unlockNV     = NULL;
//     pfn->doNext       = NULL;
//     pfn->expectComp   = &NVOCMP_expectCompApi;
//     pfn->eraseNV      = &NVOCMP_eraseNvApi;
//     pfn->getFreeNV    = &NVOCMP_getFreeNvApi;
// #ifdef ENABLE_SANITY_CHECK
//     pfn->sanityCheck  = &NVOCMP_sanityCheckApi;
// #endif
}

/*
 * Implementation of rfDriverCallbackAntennaSwitching for custom PCB
 * This function handles RF antenna switching events
 */
void rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void *arg)
{
    // For our custom PCB implementation
    if (events & RF_GlobalEventRadioSetup) {
        // When radio is being set up, configure the antenna pins
        // Set both pins to be controlled by the RF Core
        GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW, IOC_PORT_RFC_GPO0);
        GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW, IOC_PORT_RFC_GPO1);
    }

    if (events & RF_GlobalEventRadioPowerDown) {
        // When radio is being torn down, release the pins from RF Core control
        GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW, IOC_PORT_GPIO);
        GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW, IOC_PORT_GPIO);
    }
}
