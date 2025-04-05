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
uint8_t autoConnect = AUTOCONNECT_DISABLE;

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
            Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "AutoConnect turned off: Max connection reached.");
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