#include "../../common/common.h"

Task_Struct g_DisplayDriverTaskStruct;
Task_Handle g_displayDriverTaskHandle;
uint8_t g_displayDriverTaskStack[DISPLAY_DRIVER_TASK_STACK_SIZE];

static void spiWriteFT813(uint8_t *data, size_t len) {
    SPI_Transaction transaction;
    transaction.count = len;
    transaction.txBuf = data;
    transaction.rxBuf = NULL;

    GPIO_write(TFT_CS, 0);                     // Pull CS low to start transaction
    SPI_transfer(spiHandle, &transaction);     // Use shared handle from common.c
    GPIO_write(TFT_CS, 1);                     // Pull CS high to end transaction
}

void initDisplay(void) {
    printf("Initializing FT813 Display...\n");

    // Placeholder command — FT813 requires the correct sequence to be implemented here
    uint8_t Command[] = { 0x00, 0x00, 0x00 }; //dummy statements
    printf("hello");
    //Mutex_pend(spiMutex, BIOS_WAIT_FOREVER);
    spiWriteFT813(Command, sizeof(Command));
    //Mutex_post(spiMutex);

    printf("FT813 Initialized.\n");
}

void renderDisplay(int heartRate, int respirationRate) {
    printf("HR: %d  |  RR: %d\n", heartRate, respirationRate);

    if (heartRate < LOW_HEART_RATE_THRESHOLD_BPM || heartRate > HIGH_HEART_RATE_THRESHOLD_BPM ||
        respirationRate < LOW_RESPIRATORY_RATE_THRESHOLD_BRPM || respirationRate > HIGH_RESPIRATORY_RATE_THRESHOLD_BRPM) {
        printf("ALARM: Abnormal vitals detected!\n");
    }

    // We need to replace printf() with FT813 draw-text commands once graphics integration starts
}

void displayDriver_executeTask(UArg arg0, UArg arg1) {
    (void)arg0;
    (void)arg1;

    printf("Entering displayDriver_executeTask()...\n");
    initDisplay();

    static int loopCounter = 0;

    while (1) {
        // Placeholder test values — replace with real data later
        int testHeartRate = 110;
        int testRespirationRate = 35;

        // Optional: task loop counter for debugging
        loopCounter++;
        printf("Display Loop Count: %d\n", loopCounter);

        // Safe SPI access using mutex
        //Mutex_pend(spiMutex, BIOS_WAIT_FOREVER);
        renderDisplay(testHeartRate, testRespirationRate);
        //Mutex_post(spiMutex);

        Task_sleep(g_taskSleepDuration);  // Sleep for configured duration
    }
}

Task_Handle displayDriver_constructTask(){
    // Construct task
    Task_Params TaskParams;

    Task_Params_init(&TaskParams);
    TaskParams.stack = g_displayDriverTaskStack;
    TaskParams.stackSize = DISPLAY_DRIVER_TASK_STACK_SIZE;
    TaskParams.priority = DISPLAY_DRIVER_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_DisplayDriverTaskStruct, displayDriver_executeTask, &TaskParams, NULL);
    g_displayDriverTaskHandle = (Task_Handle)&g_DisplayDriverTaskStruct;
    return (Task_Handle)&g_DisplayDriverTaskStruct;
}

/*void displayDriver_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    int i  = 0;

    printf("Entering displayDriver_executeTask()...\n");
    while (1){
        i++;
        printf("displayDriver Count: %d\n", i);

        // TODO: Add display driver code here.

        Task_sleep(g_taskSleepDuration);
    }
}*/
