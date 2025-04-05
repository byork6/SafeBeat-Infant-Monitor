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

// Auto connect Disabled/Enabled {0 - Disabled, 1- Group A , 2-Group B, ...}
static uint8_t autoConnect = AUTOCONNECT_DISABLE;

//Number of group members found
static uint8_t numGroupMembers = 0;

//AutoConnect ADV data filter according to local name short
static uint8_t acGroup[4] = {
  0x03,
  GAP_ADTYPE_LOCAL_NAME_SHORT,
  'G',
  'A'
 };

//AutoConnect Group list
static osal_list_list groupList;

// Current Random Private Address
static uint8 rpa[B_ADDR_LEN] = {0};

// Value to write
static uint8_t charVal = 0;

// Accept or reject L2CAP connection parameter update request
static bool acceptParamUpdateReq = true;

// Discovery state
static uint8_t discState = BLE_DISC_STATE_IDLE;

// Discovered service start and end handle
static uint16_t svcStartHdl = 0;
static uint16_t svcEndHdl = 0;

// Connection handle of current connection
static uint16_t scConnHandle = LINKDB_CONNHANDLE_INVALID;

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

    // Init Code from example
    BLECentral_init();

    ///// Might be able to delete /////
    // ICall_Errno errno;
    // ICall_ServiceEnum src;
    // ICall_EntityID dest;
    // ICall_Hdr *pMsg = NULL;

    printf("Entering bleCentral_executeTask()...\n");
    while (1) {
        i++;
        printf("BLE Central Count: %d\n", i);

        ///////////////////////////// START OF EXAMPLE LOOP /////////////////////////////
        uint32_t events;

        events = Event_pend(syncEvent, Event_Id_NONE, SC_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);

        if (events){
          ICall_EntityID dest;
          ICall_ServiceEnum src;
          ICall_HciExtEvt *pMsg = NULL;

          if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS){
            uint8 safeToDealloc = TRUE;

            if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity)){
              ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

              // Check for BLE stack events first
              if (pEvt->signature != 0xffff){
                // Process inter-task message
                safeToDealloc = SimpleCentral_processStackMsg((ICall_Hdr *)pMsg);
              }
            }

            if (pMsg && safeToDealloc){
              ICall_freeMsg(pMsg);
            }
          }

          // If RTOS queue is not empty, process app message
          if (events & SC_QUEUE_EVT){
            scEvt_t *pMsg;
            while ((pMsg = (scEvt_t *)Util_dequeueMsg(appMsgQueue))){
              // Process message
              SimpleCentral_processAppMsg(pMsg);

              // Free the space from the message
              ICall_free(pMsg);
            }
          }
        }
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

uint8_t SimpleCentral_processStackMsg(ICall_Hdr *pMsg){
    uint8_t safeToDealloc = TRUE;

    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg status=%d, event=0x%x\n", pMsg->status, pMsg->event);

    switch (pMsg->event){
        case GAP_MSG_EVENT:
            SimpleCentral_processGapMsg((gapEventHdr_t*) pMsg);
            break;
        case GATT_MSG_EVENT:
            SimpleCentral_processGATTMsg((gattMsgEvent_t *)pMsg);
            break;
        case HCI_GAP_EVENT_EVENT:{
            // Process HCI message
            switch (pMsg->status){
                case HCI_COMMAND_COMPLETE_EVENT_CODE:
                    SimpleCentral_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
                    break;
                case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
                    AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
                    break;
                // HCI Commands Events
                case HCI_COMMAND_STATUS_EVENT_CODE:{
                    hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;
                    switch ( pMyMsg->cmdOpcode ){
                        case HCI_LE_SET_PHY:{
                            if (pMyMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE){
                                printf("PHY Change failure, peer does not support this");
                            }
                            else{
                                printf("PHY Update Status: 0x%02x", pMyMsg->cmdStatus);
                            }
                        }
                        break;
                        case HCI_DISCONNECT:
                            break;
                        default:{
                            printf("Unknown Cmd Status: 0x%04x::0x%02x", pMyMsg->cmdOpcode, pMyMsg->cmdStatus);
                        }
                        break;
                    }
                }
                break;
                // LE Events
                case HCI_LE_EVENT_CODE:{
                    hciEvt_BLEPhyUpdateComplete_t *pPUC = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;
                    if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT){
                        if (pPUC->status != SUCCESS){
                            printf("%s: PHY change failure", SimpleCentral_getConnAddrStr(pPUC->connHandle));
                        }
                        else{
                            printf("%s: PHY updated to %s", SimpleCentral_getConnAddrStr(pPUC->connHandle), 
                                // Only symmetrical PHY is supported.
                                // rxPhy should be equal to txPhy.
                                (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1 Mbps" :
                                (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2 Mbps" :
                                (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_CODED) ? "CODED" : "Unexpected PHY Value");
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case L2CAP_SIGNAL_EVENT:
            // place holder for L2CAP Connection Parameter Reply
            break;
        default:
            break;
    }
    return (safeToDealloc);
}

void SimpleCentral_processAppMsg(scEvt_t *pMsg){
    bool safeToDealloc = TRUE;

    if (pMsg->hdr.event <= APP_EVT_EVENT_MAX){
        BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=%s\n", 0, appEventStrings[pMsg->hdr.event]);
    }
    else{
        BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=0x%x\n", 0, pMsg->hdr.event);
    }
    
    switch (pMsg->hdr.event){
        case SC_EVT_ADV_REPORT:{
            GapScan_Evt_AdvRpt_t* pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->pData);
            //Auto connect is enabled
            if (autoConnect){
                if (numGroupMembers == MAX_NUM_BLE_CONNS){
                    GapScan_disable();
                    break;
                }
                //Check if advertiser is part of the group
                if (SimpleCentral_isMember(pAdvRpt->pData , acGroup, GROUP_NAME_LENGTH)){
                    groupListElem_t *tempMember;
                    //Traverse list to search if advertiser already in list.
                    for (tempMember = (groupListElem_t *)osal_list_head(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_next((osal_list_elem *)tempMember)){
                        if (osal_memcmp((uint8_t *)tempMember->addr ,(uint8_t *)pAdvRpt->addr,B_ADDR_LEN)){
                            break;
                        }
                    }
                    //If tempMemer is NULL this meams advertiser not in list.
                    if (tempMember == NULL){
                        groupListElem_t *groupMember = (groupListElem_t *)ICall_malloc(sizeof(groupListElem_t));
                        if (groupMember != NULL){
                            //Copy member's details into Member's list.
                            osal_memcpy((uint8_t *)groupMember->addr , (uint8_t *)pAdvRpt->addr,B_ADDR_LEN);
                            groupMember->addrType = pAdvRpt->addrType;
                            groupMember->status = GROUP_MEMBER_INITIALIZED;
                            groupMember->connHandle = GROUP_INITIALIZED_CONNECTION_HANDLE;
                            //Add group member into list.
                            osal_list_putHead(&groupList,(osal_list_elem *)groupMember);
                            numGroupMembers++;
                        }
                        else{
                            printf("AutoConnect: Allocation failed!");
                            break;
                        }
                    }
                }
            }
            #if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
                if (SimpleCentral_findSvcUuid(SIMPLEPROFILE_SERV_UUID, pAdvRpt->pData, pAdvRpt->dataLen)){
                    SimpleCentral_addScanInfo(pAdvRpt->addr, pAdvRpt->addrType);
                    printf("Discovered: %s", Util_convertBdAddr2Str(pAdvRpt->addr));
                }
            #else // !DEFAULT_DEV_DISC_BY_SVC_UUID
                printf("Discovered: %s", Util_convertBdAddr2Str(pAdvRpt->addr));
            #endif // DEFAULT_DEV_DISC_BY_SVC_UUID
            // Free report payload data
            if (pAdvRpt->pData != NULL){
                ICall_free(pAdvRpt->pData);
    
            }
            break;
        }
        case SC_EVT_SCAN_ENABLED:
            printf("This block was for the menu and now does nothing.");
            break;
        case SC_EVT_SCAN_DISABLED:{
            uint16_t itemsToEnable = SC_ITEM_STARTDISC | SC_ITEM_SCANPHY;
            if (autoConnect){
                itemsToEnable |= SC_ITEM_AUTOCONNECT;
                if (numGroupMembers < MAX_NUM_BLE_CONNS){
                    printf("AutoConnect: Not all members found, only %d members were found",numGroupMembers);
                }
                else{
                    printf("AutoConnect: Number of members in the group %d",numGroupMembers);
                    SimpleCentral_autoConnect();
                    if (numConn > 0){
                        // Also enable "Work with"
                        itemsToEnable |= SC_ITEM_SELECTCONN;
                    }
                }
            }
            else{
                uint8_t numReport;
                uint8_t i;
                static uint8_t* pAddrs = NULL;
                uint8_t* pAddrTemp;
                #if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
                    numReport = numScanRes;
                #else // !DEFAULT_DEV_DISC_BY_SVC_UUID
                    GapScan_Evt_AdvRpt_t advRpt;
                    numReport = ((GapScan_Evt_End_t*) (pMsg->pData))->numReport;
                #endif // DEFAULT_DEV_DISC_BY_SVC_UUID
                printf("%d devices discovered", numReport);
                if (numReport > 0){
                    // Also enable "Connect to"
                    itemsToEnable |= SC_ITEM_CONNECT;
                }
                if (numConn > 0){
                    // Also enable "Work with"
                    itemsToEnable |= SC_ITEM_SELECTCONN;
                }
                // Allocate buffer to display addresses
                if (pAddrs != NULL){
                    // A scan has been done previously, release the previously allocated buffer
                    ICall_free(pAddrs);
                }
                pAddrs = ICall_malloc(numReport * SC_ADDR_STR_SIZE);
                if (pAddrs == NULL){
                numReport = 0;
                }
                pAddrTemp = pAddrs;
                if (pAddrs != NULL){
                    for (i = 0; i < numReport; i++, pAddrTemp += SC_ADDR_STR_SIZE){
                        #if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
                            // Get the address from the list, convert it to string, and
                            // copy the string to the address buffer
                            memcpy(pAddrTemp, Util_convertBdAddr2Str(scanList[i].addr), SC_ADDR_STR_SIZE);
                        #else // !DEFAULT_DEV_DISC_BY_SVC_UUID
                            // Get the address from the report, convert it to string, and
                            // copy the string to the address buffer
                            GapScan_getAdvReport(i, &advRpt);
                            memcpy(pAddrTemp, Util_convertBdAddr2Str(advRpt.addr), SC_ADDR_STR_SIZE);
                        #endif // DEFAULT_DEV_DISC_BY_SVC_UUID
                    }
                }
                break;
            }
        }
        case SC_EVT_SVC_DISC:
            SimpleCentral_startSvcDiscovery();
            break;
        case SC_EVT_READ_RSSI:{
            uint8_t connIndex = pMsg->hdr.state;
            uint16_t connHandle = connList[connIndex].connHandle;

            // If link is still valid
            if (connHandle != LINKDB_CONNHANDLE_INVALID){
                // Restart timer
                Util_startClock(connList[connIndex].pRssiClock);

                // Read RSSI
                VOID HCI_ReadRssiCmd(connHandle);
            }

            break;
        }
        // Pairing event
        case SC_EVT_PAIR_STATE:{
            SimpleCentral_processPairState(pMsg->hdr.state, (scPairStateData_t*) (pMsg->pData));
            break;
        }
        // Passcode event
        case SC_EVT_PASSCODE_NEEDED:{
            SimpleCentral_processPasscode((scPasscodeData_t *)(pMsg->pData));
            break;
        }
        case SC_EVT_READ_RPA:{
            uint8_t* pRpaNew;

            // Read the current RPA.
            pRpaNew = GAP_GetDevAddress(FALSE);

            if (memcmp(pRpaNew, rpa, B_ADDR_LEN)){
                // If the RPA has changed, update the display
                printf("RP Addr: %s", Util_convertBdAddr2Str(pRpaNew));
                memcpy(rpa, pRpaNew, B_ADDR_LEN);
            }
            break;
        }
        // Insufficient memory
        case SC_EVT_INSUFFICIENT_MEM:{
            // We are running out of memory.
            printf("Insufficient Memory");

            // We might be in the middle of scanning, try stopping it.
            GapScan_disable();
            break;
        }
        default:
          // Do nothing.
          break;
    }
    if ((safeToDealloc == TRUE) && (pMsg->pData != NULL)){
        ICall_free(pMsg->pData);
    }
}

void SimpleCentral_autoConnect(void){
    status_t status;
    if (memberInProg == NULL){
        if (numConn < MAX_NUM_BLE_CONNS){
            groupListElem_t *tempMember = (groupListElem_t *)osal_list_head(&groupList);
            //If group member is not connected
            if ((tempMember != NULL) && (!(tempMember->status & GROUP_MEMBER_CONNECTED))){
               //Initiate a connection
               status = GapInit_connect(tempMember->addrType & MASK_ADDRTYPE_ID,tempMember->addr, DEFAULT_INIT_PHY, CONNECTION_TIMEOUT);
               if (status != SUCCESS){
                    //Couldn't create connection remove element from list and free the memory.
                    osal_list_remove(&groupList, (osal_list_elem *)tempMember);
                    ICall_free(tempMember);
                }
                else{
                    //Save pointer to connection in progress untill connection is established.
                    memberInProg = tempMember;
                }
            }
        }
        else{
            printf("AutoConnect turned off: Max connection reached.");
        }
    }
}

#ifndef Display_DISABLE_ALL
char* SimpleCentral_getConnAddrStr(uint16_t connHandle){
    uint8_t i;
    for (i = 0; i < MAX_NUM_BLE_CONNS; i++){
        if (connList[i].connHandle == connHandle){
            return Util_convertBdAddr2Str(connList[i].addr);
        }
    }
   return NULL;
}
#endif

uint8_t SimpleCentral_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len){
    if (osal_memcmp((uint8_t *)advData, (uint8_t *)groupName, len)){
        return TRUE;
    }
    return FALSE;
}

void SimpleCentral_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg){
      switch (pMsg->cmdOpcode){
        case HCI_READ_RSSI:{
            #ifndef Display_DISABLE_ALL
                uint16_t connHandle = BUILD_UINT16(pMsg->pReturnParam[1],
                                             pMsg->pReturnParam[2]);
                int8 rssi = (int8)pMsg->pReturnParam[3];

                printf("%s: RSSI %d dBm", SimpleCentral_getConnAddrStr(connHandle), rssi);
            #endif
            break;
        }
        default:
          break;
    }
}

void SimpleCentral_processGATTMsg(gattMsgEvent_t *pMsg){
    if (linkDB_Up(pMsg->connHandle)){
        // See if GATT server was unable to transmit an ATT response
        if (pMsg->hdr.status == blePending){
            // No HCI buffer was available. App can try to retransmit the response
            // on the next connection event. Drop it for now.
            printf("ATT Rsp dropped %d", pMsg->method);
        }
        else if ((pMsg->method == ATT_READ_RSP)   || ((pMsg->method == ATT_ERROR_RSP) && (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ))){
            if (pMsg->method == ATT_ERROR_RSP){
                printf("Read Error %d", pMsg->msg.errorRsp.errCode);
            }
            else{
                // After a successful read, display the read value
                printf("Read rsp: 0x%02x", pMsg->msg.readRsp.pValue[0]);
            }
        }
        else if ((pMsg->method == ATT_WRITE_RSP)  || ((pMsg->method == ATT_ERROR_RSP) && (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ))){
            if (pMsg->method == ATT_ERROR_RSP){
                printf("Write Error %d", pMsg->msg.errorRsp.errCode);
            }
            else{
                // After a successful write, display the value that was written and
                // increment value
                printf("Write sent: 0x%02x", charVal);
            }
        }
        else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT){
            // ATT request-response or indication-confirmation flow control is
            // violated. All subsequent ATT requests or indications will be dropped.
            // The app is informed in case it wants to drop the connection.
            // Display the opcode of the message that caused the violation.
            printf("FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
        }
        else if (pMsg->method == ATT_MTU_UPDATED_EVENT){
            // MTU size updated
            printf("MTU Size: %d", pMsg->msg.mtuEvt.MTU);
        }
        else if (discState != BLE_DISC_STATE_IDLE){
            SimpleCentral_processGATTDiscEvent(pMsg);
        }
    } 
    // else - in case a GATT message came after a connection has dropped, ignore it.
    // Needed only for ATT Protocol messages
    GATT_bm_free(&pMsg->msg, pMsg->method);
}

void SimpleCentral_processGapMsg(gapEventHdr_t *pMsg){
  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      uint8_t temp8;
      uint16_t temp16;
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_DEVICE_INIT_DONE_EVENT", 0);
      // Setup scanning
      // For more information, see the GAP section in the User's Guide:
      // http://software-dl.ti.com/lprf/ble5stack-latest/

      // Register callback to process Scanner events
      GapScan_registerCb(SimpleCentral_scanCb, NULL);

      // Set Scanner Event Mask
      GapScan_setEventMask(GAP_EVT_SCAN_ENABLED | GAP_EVT_SCAN_DISABLED |
                           GAP_EVT_ADV_REPORT);

      // Set Scan PHY parameters
      GapScan_setPhyParams(DEFAULT_SCAN_PHY, DEFAULT_SCAN_TYPE,
                           DEFAULT_SCAN_INTERVAL, DEFAULT_SCAN_WINDOW);

      // Set Advertising report fields to keep
      temp16 = ADV_RPT_FIELDS;
      GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &temp16);
      // Set Scanning Primary PHY
      temp8 = DEFAULT_SCAN_PHY;
      GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);
      // Set LL Duplicate Filter
      temp8 = SCANNER_DUPLICATE_FILTER;
      GapScan_setParam(SCAN_PARAM_FLT_DUP, &temp8);

      // Set PDU type filter -
      // Only 'Connectable' and 'Complete' packets are desired.
      // It doesn't matter if received packets are
      // whether Scannable or Non-Scannable, whether Directed or Undirected,
      // whether Scan_Rsp's or Advertisements, and whether Legacy or Extended.
      temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GapScan_setParam", 0);
      GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);

	  // Set initiating PHY parameters
      GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MIN,
						  INIT_PHYPARAM_MIN_CONN_INT);
      GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MAX,
						  INIT_PHYPARAM_MAX_CONN_INT);

      scMaxPduSize = pPkt->dataPktLen;

      printf( "Initialized");
      printf("Num Conns: %d", numConn);

      // Display device address
      printf("%s Addr: %s", (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr));

      if (addrMode > ADDRMODE_RANDOM)
      {
        // Update the current RPA.
        memcpy(rpa, GAP_GetDevAddress(FALSE), B_ADDR_LEN);

        printf("RP Addr: %s", Util_convertBdAddr2Str(rpa));

        // Create one-shot clock for RPA check event.
        Util_constructClock(&clkRpaRead, SimpleCentral_clockHandler,
                            READ_RPA_PERIOD, 0, true, SC_EVT_READ_RPA);
      }
      break;
    }

    case GAP_CONNECTING_CANCELLED_EVENT:
    {
      uint16_t itemsToEnable = SC_ITEM_SCANPHY | SC_ITEM_STARTDISC |
                               SC_ITEM_CONNECT | SC_ITEM_AUTOCONNECT;
      if (autoConnect)
      {
        if (memberInProg != NULL)
        {
          //Remove node from member's group and free its memory.
          osal_list_remove(&groupList, (osal_list_elem *)memberInProg);
          ICall_free(memberInProg);
          numGroupMembers--;
          memberInProg = NULL;
        }
        printf("AutoConnect: Number of members in the group %d",numGroupMembers);
        //Keep on connecting to the remaining members in the list
        SimpleCentral_autoConnect();
      }

      if (numConn > 0)
      {
        itemsToEnable |= SC_ITEM_SELECTCONN;
      }
      printf("Conneting attempt cancelled");

      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      uint16_t connHandle = ((gapEstLinkReqEvent_t*) pMsg)->connectionHandle;
      uint8_t* pAddr = ((gapEstLinkReqEvent_t*) pMsg)->devAddr;
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_LINK_ESTABLISHED_EVENT", 0);
      if (autoConnect)
      {
        if (memberInProg != NULL)
        {
          if (osal_memcmp((uint8_t *)pAddr, (uint8_t *)memberInProg->addr, B_ADDR_LEN))
          {
            //Move the connected member to the tail of the list.
            osal_list_remove(&groupList,(osal_list_elem *)memberInProg);
            osal_list_put(&groupList,(osal_list_elem *)memberInProg);
            //Set the connected bit.;
            memberInProg->status |= GROUP_MEMBER_CONNECTED;
            //Store the connection handle.
            memberInProg->connHandle = connHandle;
            memberInProg = NULL;
          }
        }
      }
      uint8_t  connIndex;
      uint32_t itemsToDisable = SC_ITEM_STOPDISC | SC_ITEM_CANCELCONN;
      uint8_t* pStrAddr;
      uint8_t i;
      uint8_t numConnectable = 0;
      uint8_t pairMode = 0;

      // Add this connection info to the list
      connIndex = SimpleCentral_addConnInfo(connHandle, pAddr);

      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      connList[connIndex].charHandle = 0;

      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);
      
      printf("Connected to %s", pStrAddr);
      printf("Num Conns: %d", numConn);

      // Disable "Connect To" until another discovery is performed
      itemsToDisable |= SC_ITEM_CONNECT;

      // If we already have maximum allowed number of connections,
      // disable device discovery and additional connection making.
      if (numConn >= MAX_NUM_BLE_CONNS)
      {
        itemsToDisable |= SC_ITEM_SCANPHY | SC_ITEM_STARTDISC;
      }

      GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

      if ((autoConnect) && (pairMode != GAPBOND_PAIRING_MODE_INITIATE))
      {
        SimpleCentral_autoConnect();
      }
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
      uint8_t connIndex;
      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_TERMINATED_EVENT");
      uint32_t itemsToEnable = SC_ITEM_STARTDISC | SC_ITEM_SCANPHY | SC_ITEM_AUTOCONNECT;
      uint8_t* pStrAddr;
      uint8_t i;
      uint8_t numConnectable = 0;
      uint16_t connHandle = ((gapTerminateLinkEvent_t*) pMsg)->connectionHandle;
      if (autoConnect)
      {
        groupListElem_t *tempMember;
        //Traverse from tail to head because of the sorting which put the connected at the end of the list.
        for (tempMember = (groupListElem_t *)osal_list_tail(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_prev((osal_list_elem *)tempMember))
        {
          if (tempMember->connHandle == connHandle)
          {
            //Move disconnected member to the head of the list for next connection.
            osal_list_remove(&groupList,(osal_list_elem *)tempMember);
            osal_list_putHead(&groupList,(osal_list_elem *)tempMember);
            //Clear the connected flag.
            tempMember->status &= ~GROUP_MEMBER_CONNECTED;
            //Clear the connnection handle.
            tempMember->connHandle = GROUP_INITIALIZED_CONNECTION_HANDLE;
          }
        }
      }
      // Cancel timers
      SimpleCentral_CancelRssi(connHandle);

      // Mark this connection deleted in the connected device list.
      connIndex = SimpleCentral_removeConnInfo(connHandle);
      if (autoConnect)
      {
        SimpleCentral_autoConnect();
      }
      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);

      printf("%s is disconnected", pStrAddr);
      printf("Num Conns: %d", numConn);

      if (numConn > 0)
      {
        // There still is an active connection to select
        itemsToEnable |= SC_ITEM_SELECTCONN;
      }

      break;
    }

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
    {
      gapUpdateLinkParamReqReply_t rsp;
      gapUpdateLinkParamReq_t *pReq;

      pReq = &((gapUpdateLinkParamReqEvent_t *)pMsg)->req;

      rsp.connectionHandle = pReq->connectionHandle;
      rsp.signalIdentifier = pReq->signalIdentifier;

      if (acceptParamUpdateReq)
      {
        rsp.intervalMin = pReq->intervalMin;
        rsp.intervalMax = pReq->intervalMax;
        rsp.connLatency = pReq->connLatency;
        rsp.connTimeout = pReq->connTimeout;
        rsp.accepted = TRUE;
      }
      else
      {
        // Reject the request.
        rsp.accepted = FALSE;
      }

      // Send Reply
      VOID GAP_UpdateLinkParamReqReply(&rsp);

      if (autoConnect)
      {
        SimpleCentral_autoConnect();
      }

      break;
    }

    case GAP_LINK_PARAM_UPDATE_EVENT:
    {
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;
      // Get the address from the connection handle
      linkDBInfo_t linkInfo;

      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_PARAM_UPDATE_EVENT");
      if (linkDB_GetInfo(pPkt->connectionHandle, &linkInfo) ==  SUCCESS)
      {
        if(pPkt->status == SUCCESS)
        {
          printf("Updated: %s, connTimeout:%d", Util_convertBdAddr2Str(linkInfo.addr), linkInfo.connTimeout*CONN_TIMEOUT_MS_CONVERSION);
        }
        else
        {
          // Display the address of the connection update failure
          printf("Update Failed 0x%h: %s", pPkt->opcode, Util_convertBdAddr2Str(linkInfo.addr));
        }
      }

      if (autoConnect)
      {
        SimpleCentral_autoConnect();
      }

      break;
    }

#if defined ( NOTIFY_PARAM_UPDATE_RJCT )

    case GAP_LINK_PARAM_UPDATE_REJECT_EVENT:
    {
      linkDBInfo_t linkInfo;
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

      // Get the address from the connection handle
      linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

      // Display the address of the connection update failure
      printf("Peer Device's Update Request Rejected 0x%h: %s", pPkt->opcode, Util_convertBdAddr2Str(linkInfo.addr));
      break;
    }
#endif

    default:
      break;
  }
}

void SimpleCentral_processPairState(uint8_t state, scPairStateData_t* pPairData){
  uint8_t status = pPairData->status;
  uint8_t pairMode = 0;

  if (state == GAPBOND_PAIRING_STATE_STARTED)
  {
    printf("Pairing started");
  }
  else if (state == GAPBOND_PAIRING_STATE_COMPLETE)
  {
    if (status == SUCCESS)
    {
      linkDBInfo_t linkInfo;

      printf("Pairing success");

      if (linkDB_GetInfo(pPairData->connHandle, &linkInfo) == SUCCESS)
      {
        // If the peer was using private address, update with ID address
        if ((linkInfo.addrType == ADDRTYPE_PUBLIC_ID ||
             linkInfo.addrType == ADDRTYPE_RANDOM_ID) &&
             !Util_isBufSet(linkInfo.addrPriv, 0, B_ADDR_LEN))
        {
          // Update the address of the peer to the ID address
          printf("Addr updated: %s", Util_convertBdAddr2Str(linkInfo.addr));
          // Update the connection list with the ID address
          uint8_t i = SimpleCentral_getConnIndex(pPairData->connHandle);

          SIMPLECENTRAL_ASSERT(i < MAX_NUM_BLE_CONNS);
          memcpy(connList[i].addr, linkInfo.addr, B_ADDR_LEN);
        }
      }
    }
    else
    {
      printf("Pairing fail: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

    if ((autoConnect) && (pairMode == GAPBOND_PAIRING_MODE_INITIATE))
    {
      SimpleCentral_autoConnect();
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_ENCRYPTED)
  {
    if (status == SUCCESS)
    {
      printf("Encryption success");
    }
    else
    {
      printf("Encryption failed: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

    if ((autoConnect) && (pairMode == GAPBOND_PAIRING_MODE_INITIATE))
    {
      SimpleCentral_autoConnect();
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_BOND_SAVED)
  {
    if (status == SUCCESS)
    {
      printf("Bond save success");
    }
    else
    {
      printf("Bond save failed: %d", status);
    }
  }
}

void SimpleCentral_processPasscode(scPasscodeData_t *pData){
    // Display passcode to user
    if (pData->uiOutputs != 0){
        printf("Passcode: %d", B_APP_DEFAULT_PASSCODE);
    }
    // Send passcode response
    GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

void SimpleCentral_startSvcDiscovery(void){
    attExchangeMTUReq_t req;
    // Initialize cached handles
    svcStartHdl = svcEndHdl = 0;
    discState = BLE_DISC_STATE_MTU;
    // Discover GATT Server's Rx MTU size
    req.clientRxMTU = scMaxPduSize - L2CAP_HDR_SIZE;
    // ATT MTU size should be set to the minimum of the Client Rx MTU
    // and Server Rx MTU values
    VOID GATT_ExchangeMTU(scConnHandle, &req, selfEntity);
}

void SimpleCentral_scanCb(uint32_t evt, void* pMsg, uintptr_t arg){
    uint8_t event;

    if (evt & GAP_EVT_ADV_REPORT){
      event = SC_EVT_ADV_REPORT;
    }
    else if (evt & GAP_EVT_SCAN_ENABLED){
      event = SC_EVT_SCAN_ENABLED;
    }
    else if (evt & GAP_EVT_SCAN_DISABLED){
      event = SC_EVT_SCAN_DISABLED;
    }
    else if (evt & GAP_EVT_INSUFFICIENT_MEMORY){
      event = SC_EVT_INSUFFICIENT_MEM;
    }
    else{
      return;
    }

    if(SimpleCentral_enqueueMsg(event, SUCCESS, pMsg) != SUCCESS){
      ICall_free(pMsg);
    }
}

void SimpleCentral_clockHandler(UArg arg){
  uint8_t evtId = (uint8_t) (arg & 0xFF);

  switch (evtId)
  {
    case SC_EVT_READ_RSSI:
      SimpleCentral_enqueueMsg(SC_EVT_READ_RSSI, (uint8_t) (arg >> 8) , NULL);
      break;

    case SC_EVT_READ_RPA:
      // Restart timer
      Util_startClock(&clkRpaRead);
      // Let the application handle the event
      SimpleCentral_enqueueMsg(SC_EVT_READ_RPA, 0, NULL);
      break;

    default:
      break;
  }
}

status_t SimpleCentral_CancelRssi(uint16_t connHandle){
  uint8_t connIndex = SimpleCentral_getConnIndex(connHandle);

  // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
  SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // If already running
  if (connList[connIndex].pRssiClock == NULL)
  {
    return bleIncorrectMode;
  }

  // Stop timer
  Util_stopClock(connList[connIndex].pRssiClock);

  // Destroy the clock object
  Clock_destruct(connList[connIndex].pRssiClock);

  // Free clock struct
  ICall_free(connList[connIndex].pRssiClock);
  connList[connIndex].pRssiClock = NULL;

  Display_clearLine(dispHandle, SC_ROW_ANY_CONN);

  return SUCCESS;
}

uint8_t SimpleCentral_addConnInfo(uint16_t connHandle, uint8_t *pAddr){
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
      // Found available entry to put a new connection info in
      connList[i].connHandle = connHandle;
      memcpy(connList[i].addr, pAddr, B_ADDR_LEN);
      numConn++;

      break;
    }
  }

  return i;
}

uint8_t SimpleCentral_getConnIndex(uint16_t connHandle){
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      break;
    }
  }

  return i;
}