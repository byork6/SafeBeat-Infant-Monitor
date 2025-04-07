#include "common.h"

// VARIABLE DECLARATIONS
// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;
// Address Mode
static uint8_t addrMode = DEFAULT_ADDRESS_MODE;// BLE user defined configuration

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

    printf("Initializing BLE Central...\n");
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
    GATT_RegisterForMsgs(ICALL_SERVICE_CLASS_BLE);

    // Set the GAP Role (this sets the device as Central)
    printf("GAP_DeviceInit\n");
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, pRandomAddress);
}
