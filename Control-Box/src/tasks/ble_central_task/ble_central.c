#include "../../common/common.h"

static uint8_t heartRate = 7;
static uint8_t respRate = 6;

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

    BLECentral_init();

    while (1) {
        BLECentral_readMeasurements();
        Task_sleep(g_taskSleepDuration);
    }
}

void BLECentral_init(void) {
    BLE_init();
    BLE_startScanning();
    BLE_connectToPeripheral(SERVICE_UUID);
}

void BLECentral_readMeasurements(void) {
    BLE_readCharacteristic(HEART_RATE_UUID, &heartRate, sizeof(heartRate));
    BLE_readCharacteristic(RESP_RATE_UUID, &respRate, sizeof(respRate));

    printf("Heart Rate: %d bpm, Resp Rate: %d breaths/min\n", heartRate, respRate);
}