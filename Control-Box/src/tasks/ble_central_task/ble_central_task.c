#include "common.h"

// VARIABLE DECLARATIONS
static uint8_t addrMode = DEFAULT_ADDRESS_MODE;

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
    
    bleCentral_init();

    printf("Entering bleCentral_executeTask()...\n");
    while (1) {
        i++;
        printf("BLE Central Count: %d\n", i);

        // TODO: Add code here

        Task_sleep(g_taskSleepDuration);
    }
}

void bleCentral_init(void){
    // Register with stack to receive GAP and GATT messages
    ICall_registerApp(&selfEntity, NULL);

    // Initialize GATT
    GATT_InitClient();
    GATT_RegisterForMsgs(ICALL_SERVICE_CLASS_BLE);

    // Set the GAP Role (this sets the device as Central)
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, pRandomAddress);
}

// void  *MAP_l2capFindLocalCID( uint16 CID ){

// }

// void MAP_l2capGetCoChannelInfo(void *pCoC, void *pInfo ){

// }