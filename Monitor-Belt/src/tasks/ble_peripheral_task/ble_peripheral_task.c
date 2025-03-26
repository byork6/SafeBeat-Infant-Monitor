#include "common.h"

static uint8_t selfEntity;
static uint16_t scanResFields = GAP_ADTYPE_FLAGS | GAP_ADTYPE_16BIT_MORE | GAP_ADTYPE_LOCAL_NAME_COMPLETE;

Task_Handle blePeripheral_constructTask(){
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_blePeripheralTaskStack;
    TaskParams.stackSize = BLE_PERIPHERAL_TASK_STACK_SIZE;
    TaskParams.priority = BLE_PERIPHERAL_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_BlePeripheralTaskStruct, blePeripheral_executeTask, &TaskParams, NULL);
    return (Task_Handle)&g_BlePeripheralTaskStruct;
}

void blePeripheral_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    ICall_Errno errno;
    ICall_ServiceEnum src;
    ICall_EntityID dest;
    ICall_Hdr *pMsg = NULL;

    BLECentral_init();

    while (1) {
        // Wait (block) for BLE stack or system message
        errno = ICall_wait(ICALL_TIMEOUT_FOREVER);
        if (errno == ICALL_ERRNO_SUCCESS) {
            if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS) {
                if ((src == ICALL_SERVICE_CLASS_BLE) && (pMsg->event == GATT_MSG_EVENT)) {
                    gattMsgEvent_t *gattMsg = (gattMsgEvent_t *)pMsg;
                    if (gattMsg->method == ATT_READ_RSP) {
                        // ✅ Access the read data here:
                        uint8_t *value = gattMsg->msg.readRsp.pValue;
                        uint16_t len = gattMsg->msg.readRsp.len;
                        // For debugging
                        printf("GATT Read Response: Len = %d\n", len);
                    }
                    // Free BLE stack buffers
                    GATT_bm_free(&gattMsg->msg, gattMsg->method);
                }
                // Free ICall wrapper
                ICall_freeMsg(pMsg);
            }
        }
        Task_sleep(g_taskSleepDuration);
    }
}

void BLECentral_init(void){
    ICall_init();
    ICall_createRemoteTasks();  // Create BLE remote tasks

    selfEntity = ICall_registerApp(NULL, NULL);

    // Initialize GAP Central Role
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, ADDRMODE_PUBLIC, NULL);
}

void BLECentral_startScanning(void){
    GapScan_setPhyParams(DEFAULT_SCAN_PHY, SCAN_TYPE_PASSIVE, SCAN_INTERVAL, SCAN_WINDOW);
    GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &scanResFields);
    GapScan_registerCb(scanCallback, NULL);

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
    GapInit_connect(peerAddr, addrType, DEFAULT_INIT_PHY, 0);
}

void BLE_readCharacteristic(uint16_t connHandle, uint16_t charHandle){
    attReadReq_t req;
    req.handle = charHandle;

    GATT_ReadCharValue(connHandle, &req, selfEntity);
}