#include "common.h"

// GLOBAL VARIABLES
static ICall_EntityID selfEntity;
static uint16_t scanResFields = GAP_ADTYPE_FLAGS | GAP_ADTYPE_16BIT_MORE | GAP_ADTYPE_LOCAL_NAME_COMPLETE;

// Number of connected devices
static uint8_t numConn = 0;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

// Bond Manager Callbacks
static gapBondCBs_t bondMgrCBs =
{
  SimpleCentral_passcodeCb, // Passcode callback
  SimpleCentral_pairStateCb // Pairing/Bonding state Callback
};

// Define user config
#ifndef USE_DEFAULT_USER_CFG
#include "ble_user_config.h"
// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;
#endif // USE_DEFAULT_USER_CFG

// FUNCTION DEFINITIONS
Task_Handle bleCentral_constructTask(){
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
    static int i = 0;
    
    ///////////////////////////// START OF EXAMPLE CODE INIT /////////////////////////////
    // NOTE: Everything from here until the while loop is from the Simple_Central Example.
    // All of the code below is for the BLE initializtion.
    BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- init ", BLE_CENTRAL_TASK_PRIORITY);
    printf("APP --- init\n");

    // ******************************************************************
    // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
    // ******************************************************************
    // Register the current thread as an ICall dispatcher application
    // so that the application can send and receive messages.
    ICall_registerApp(&selfEntity, &syncEvent);

    // Create an RTOS queue for message from profile to be sent to app.
    appMsgQueue = Util_constructQueue(&appMsg);

    // Initialize internal data
    for (i = 0; i < MAX_NUM_BLE_CONNS; i++){
      connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      connList[i].pRssiClock = NULL;
    }

    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,(void *)attDeviceName);

    //Set default values for Data Length Extension
    //Extended Data Length Feature is already enabled by default
    //in build_config.opt in stack project.
    {
      //Change initial values of RX/TX PDU and Time, RX is set to max. by default(251 octets, 2120us)
      #define APP_SUGGESTED_RX_PDU_SIZE 251     //default is 251 octets(RX)
      #define APP_SUGGESTED_RX_TIME     17000   //default is 17000us(RX)
      #define APP_SUGGESTED_TX_PDU_SIZE 27      //default is 27 octets(TX)
      #define APP_SUGGESTED_TX_TIME     328     //default is 328us(TX)

      //This API is documented in hci.h
      //See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
      //http://software-dl.ti.com/lprf/ble5stack-latest/
      HCI_EXT_SetMaxDataLenCmd(APP_SUGGESTED_TX_PDU_SIZE, APP_SUGGESTED_TX_TIME, APP_SUGGESTED_RX_PDU_SIZE, APP_SUGGESTED_RX_TIME);
    }

    // Initialize GATT Client
    VOID GATT_InitClient();

    // Register to receive incoming ATT Indications/Notifications
    GATT_RegisterForInd(selfEntity);

    // Initialize GATT attributes
    GGS_AddService(GAP_SERVICE);               // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES); // GATT attributes

    // Register for GATT local events and ATT Responses pending for transmission
    GATT_RegisterForMsgs(selfEntity);

    // Set Bond Manager parameters
    setBondManagerParameters();

    // Start Bond Manager and register callback
    // This must be done before initialing the GAP layer
    VOID GAPBondMgr_Register(&bondMgrCBs);

    // Accept all parameter update requests
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, GAP_UPDATE_REQ_ACCEPT_ALL);

    // Register with GAP for HCI/Host messages (for RSSI)
    GAP_RegisterForMsgs(selfEntity);

    BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- call GAP_DeviceInit", GAP_PROFILE_CENTRAL);
    // Initialize GAP layer for Central role and register to receive GAP events
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, &pRandomAddress);

    ///////////////////////////// EMD OF EXAMPLE CODE INIT /////////////////////////////

    ///// MY INIT CODE --- Might be able to delete /////
    // ICall_Errno errno;
    // ICall_ServiceEnum src;
    // ICall_EntityID dest;
    // ICall_Hdr *pMsg = NULL;

    // BLECentral_init();

    printf("Entering bleCentral_executeTask()...\n");
    while (1) {
        i++;
        printf("BLE Central Count: %d\n", i);

        ///////////////////////////// START OF EXAMPLE LOOP /////////////////////////////
        // TODO: Add application loop from example here.
        ////////////////////////////// END OF EXAMPLE LOOP //////////////////////////////

        ////////////////////////////// MY CODE STARTS HERE //////////////////////////////
        // // Wait (block) for BLE stack or system message
        // errno = ICall_wait(ICALL_TIMEOUT_FOREVER);
        // if (errno == ICALL_ERRNO_SUCCESS) {
        //     if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS) {
        //         if ((src == ICALL_SERVICE_CLASS_BLE) && (pMsg->event == GATT_MSG_EVENT)) {
        //             gattMsgEvent_t *gattMsg = (gattMsgEvent_t *)pMsg;
        //             if (gattMsg->method == ATT_READ_RSP) {
        //                 // Access the read data here:
        //                 uint8_t *value = gattMsg->msg.readRsp.pValue;
        //                 uint16_t len = gattMsg->msg.readRsp.len;
        //                 // For debugging
        //                 printf("GATT Read Response: Len = %d\n", len);
        //             }
        //             // Free BLE stack buffers
        //             GATT_bm_free(&gattMsg->msg, gattMsg->method);
        //         }
        //         // Free ICall wrapper
        //         ICall_freeMsg(pMsg);
        //     }
        // }
        // Task_sleep(g_taskSleepDuration);
        ////////////////////////////// MY CODE ENDS HERE //////////////////////////////

    }
}

void BLECentral_init(void){
    // TODO: Add ex code init block here.
}

void BLECentral_startScanning(void){
    GapScan_setPhyParams(DEFAULT_SCAN_PHY, SCAN_TYPE_PASSIVE, SCAN_INTERVAL, SCAN_WINDOW);
    GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &scanResFields);
    GapScan_registerCb(scanCallback, (uintptr_t)NULL);

    GapScan_enable(0, 0, 0);  // 0=continuous scan, 0 timeout, 0 numAdvReports
}

void scanCallback(uint32_t event, void *ptrBuf, uintptr_t arg){
    if (event == GAP_EVT_ADV_REPORT){
        GapScan_Evt_AdvRpt_t *advRpt = (GapScan_Evt_AdvRpt_t *)ptrBuf;

        // Print or filter by address, RSSI, local name etc.
        // You can call BLECentral_connectToPeripheral() here

        ICall_free(ptrBuf);
    }
}

void BLECentral_connectToPeripheral(uint8_t *peerAddr, uint8_t addrType){
    GapInit_connect(addrType, peerAddr, DEFAULT_INIT_PHY, 0);
}

void BLE_readCharacteristic(uint16_t connHandle, uint16_t charHandle){
    attReadReq_t req;
    req.handle = charHandle;

    GATT_ReadCharValue(connHandle, &req, selfEntity);
}

void rfDriverCallbackAntennaSwitching(void) {
    // Empty stub
}

void SimpleCentral_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle, uint8_t uiInputs, uint8_t uiOutputs, uint32_t numComparison){
  scPasscodeData_t *pData = ICall_malloc(sizeof(scPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData)
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, deviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (SimpleCentral_enqueueMsg(SC_EVT_PASSCODE_NEEDED, 0,(uint8_t *) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

void SimpleCentral_pairStateCb(uint16_t connHandle, uint8_t state, uint8_t status){
  scPairStateData_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(scPairStateData_t))))
  {
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if(SimpleCentral_enqueueMsg(SC_EVT_PAIR_STATE, state, (uint8_t*) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

status_t SimpleCentral_enqueueMsg(uint8_t event, uint8_t state, int8_t *pData){
  uint8_t success;
  scEvt_t *pMsg = ICall_malloc(sizeof(scEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}