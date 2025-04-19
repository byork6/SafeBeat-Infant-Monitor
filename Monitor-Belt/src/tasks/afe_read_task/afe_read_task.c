#include "../../common/common.h"

// --- VARIABLE DEFINITIONS --- //
ECGQueue g_ecgQueue;

Semaphore_Handle g_afeDataReadyHandle;
Semaphore_Struct g_afeDataReadyStruct;

SPI_Handle g_afeSpiHandle;
SPI_Transaction g_afeSpiTransaction;
SPI_Params g_afeSpiParams;

// --- FUNCTION DEFINITIONS --- //
Task_Handle afeRead_constructTask(void){
    Task_Params TaskParams;

    // Task setup
    Task_Params_init(&TaskParams);
    TaskParams.stack = g_afeReadTaskStack;
    TaskParams.stackSize = AFE_READ_TASK_STACK_SIZE;
    TaskParams.priority = AFE_READ_TASK_PRIORITY;
    TaskParams.arg0 = 0;
    TaskParams.arg1 = 0;

    Task_construct(&g_AfeReadTaskStruct, afeRead_executeTask, &TaskParams, NULL);
    Semaphore_Params afeDataReadyParams;

    Semaphore_Params_init(&afeDataReadyParams);
    afeDataReadyParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&g_afeDataReadyStruct, 0, &afeDataReadyParams);
    g_afeDataReadyHandle = Semaphore_handle(&g_afeDataReadyStruct);

    // DRDY interrupt setup
    GPIO_setCallback(CONFIG_GPIO_ADAS_DRDY, afe_handleDRDY);
    GPIO_enableInt(CONFIG_GPIO_ADAS_DRDY);

    return (Task_Handle)&g_AfeReadTaskStruct;
}

void afeRead_executeTask(UArg arg0, UArg arg1){
    (void)arg0;
    (void)arg1;
    int i = 0;

    // Init AFE SPI
    afe_spiInit();

    // Extended frame will be longer than 6 words — we use 16 to be safe
    uint16_t txBuf[16] = {0};  // Dummy TX buffer (ADAS doesn’t require data here)
    uint16_t rxBuf[16] = {0};  // Buffer for incoming extended frame

    printf("Entering afeRead_executeTask()...\n");
    while (1){
        printf("afeRead Count: %d\n", ++i);
        // Wait for DRDY interrupt to signal new ECG+RESP data is ready
        printf("Waiting for DRDY...\n");
        Task_sleep(g_taskSleepDuration);
        Semaphore_pend(g_afeDataReadyHandle, BIOS_WAIT_FOREVER);
        printf("DRDY Triggered\n");

        g_afeSpiHandle = SPI_open(CONFIG_AFE_SPI, &g_afeSpiParams);
        g_afeSpiTransaction.count = sizeof(rxBuf);     // Full buffer size in bytes
        g_afeSpiTransaction.txBuf = txBuf;             // Dummy TX (ADAS just clocks out data)
        g_afeSpiTransaction.rxBuf = rxBuf;             // Fill with incoming data
        SPI_transfer(g_afeSpiHandle, &g_afeSpiTransaction);
        SPI_close(g_afeSpiHandle);

        // Create and enqueue the full ECG frame (for future processing)
        ECGFrame frame;
        memcpy(frame.data, rxBuf, sizeof(frame.data));
        if (!ecg_enqueue(&frame)) {
            printf("ECG queue full, frame dropped!\n");
        }

        // =============================================
        // BEGIN: Extract and log Lead II and RESP
        // =============================================

        /**
         * According to ADAS1000 datasheet (Table 55, 56):
         * Lead II = LL – RA → typically shows up in word 2
         * RESP output (impedance signal) often appears around word 4–6
         * Since we’re using 16-bit words, index them directly
         * You can adjust the index after confirming from actual data logs
         */

        int16_t leadII_raw = rxBuf[2];  // Lead II (LL - RA)
        int16_t resp_raw   = rxBuf[4];  // RESP signal

        // Optionally sign-extend to 32-bit if needed
        int32_t leadII = (int32_t)((int16_t)leadII_raw);
        int32_t resp   = (int32_t)((int16_t)resp_raw);

        // =============================================
        // END: Lead II + RESP debug
        // =============================================

        // (Optional) Print raw SPI frame for inspection — helps confirm location of data
        char spiMsg[128];
        int offset = snprintf(spiMsg, sizeof(spiMsg), "Extended Frame: ");
        
        for (int i = 0; i < 8 && offset < sizeof(spiMsg) - 6; i++) {
            offset += snprintf(&spiMsg[offset], sizeof(spiMsg) - offset, "%04X ", rxBuf[i]);
        }
        
        snprintf(&spiMsg[offset], sizeof(spiMsg) - offset, "\r\n"); // Clean line ending
        printf("%s", spiMsg);

        Task_sleep(g_taskSleepDuration);
    }
}

void afe_spiInit(void){
    SPI_Params_init(&g_afeSpiParams);
    g_afeSpiParams.frameFormat = SPI_POL1_PHA1; // Mode 3 (CPOL=1, CPHA=1)
    g_afeSpiParams.dataSize = 16;
    g_afeSpiParams.bitRate = 1000000; // 1 MHz safe default
}

void afe_writeRegister(uint8_t regAddr, uint16_t data){
    uint16_t command = ((regAddr & 0x1F) << 11) | (0 << 10) | (data & 0x3FF); // write op
    SPI_Transaction g_afeSpiTransaction;
    g_afeSpiTransaction.count = 2;
    g_afeSpiTransaction.txBuf = &command;
    g_afeSpiTransaction.rxBuf = NULL;

    SPI_transfer(g_afeSpiHandle, &g_afeSpiTransaction);
}

void afe_configureADAS(void){
    // Enable CH1–CH3 (LA, LL, RA)
    afe_writeRegister(0x01, 0x0C3); // CONFIG

    // Enable extended frame output
    afe_writeRegister(0x03, 0x001); // FRMCTL – Extended Frame

    // Internal reference
    afe_writeRegister(0x04, 0x000); // CMREFCTL

    // Enable respiration measurement
    afe_writeRegister(0x0A, 0x01F); // RESPCTL – Enable RESP on all channels

    // Optional: set respiration frequency if you want (not strictly required)
    // Register 0x0C is RESP_FREQ – set to 0x00 for 64 kHz or something safe
    afe_writeRegister(0x0C, 0x000); // 64 kHz carrier
}

void afe_handleDRDY(uint_least8_t index){
    if (index == CONFIG_GPIO_ADAS_DRDY){
        Semaphore_post(g_afeDataReadyHandle);
    }
}


// --- ECG FUNCTION DEFINITIONS --- //
void ecg_queue_init(void){
    g_ecgQueue.head = 0;
    g_ecgQueue.tail = 0;
    g_ecgQueue.count = 0;
}

bool ecg_enqueue(const ECGFrame* frame){
    if (g_ecgQueue.count >= ECG_QUEUE_SIZE) {
        return false;
    }

    g_ecgQueue.buffer[g_ecgQueue.tail] = *frame;
    g_ecgQueue.tail = (g_ecgQueue.tail + 1) % ECG_QUEUE_SIZE;
    g_ecgQueue.count++;
    return true;
}

bool ecg_dequeue(ECGFrame* out_frame){
    if (g_ecgQueue.count == 0) return false;

    memcpy(out_frame, &g_ecgQueue.buffer[g_ecgQueue.head], sizeof(ECGFrame));
    g_ecgQueue.head = (g_ecgQueue.head + 1) % ECG_QUEUE_SIZE;
    g_ecgQueue.count--;
    return true;
}

bool ecg_queue_is_full(void){
    return (g_ecgQueue.count >= ECG_QUEUE_SIZE);
}

bool ecg_queue_is_empty(void){
    return (g_ecgQueue.count == 0);
}