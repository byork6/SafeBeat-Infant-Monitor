#include "../../common/common.h"

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
    SPI_Transaction spiTransaction;

    // Extended frame will be longer than 6 words — we use 16 to be safe
    uint16_t txBuf[16] = {0};  // Dummy TX buffer (ADAS doesn’t require data here)
    uint16_t rxBuf[16] = {0};  // Buffer for incoming extended frame

    while (1){
        printf("in while loop\n");
        // // Wait for DRDY interrupt to signal new ECG+RESP data is ready
        // Semaphore_pend(g_afeDataReadyHandle, BIOS_WAIT_FOREVER);

        // // Begin SPI transaction
        // GPIO_write(CONFIG_GPIO_ADAS_CS, 0);

        // spiTransaction.count = sizeof(rxBuf);     // Full buffer size in bytes
        // spiTransaction.txBuf = txBuf;             // Dummy TX (ADAS just clocks out data)
        // spiTransaction.rxBuf = rxBuf;             // Fill with incoming data
        // SPI_transfer(afeSpiHandle, &spiTransaction);

        // // End SPI transaction
        // GPIO_write(CONFIG_GPIO_ADAS_CS, 1);

        // // Create and enqueue the full ECG frame (for future processing)
        // ECGFrame frame;
        // memcpy(frame.data, rxBuf, sizeof(frame.data));
        // if (!ecg_enqueue(&frame)) {
        //     printf("ECG queue full, frame dropped!\n");
        // }

        // // =============================================
        // // BEGIN: Extract and log Lead II and RESP
        // // =============================================

        // /**
        //  * According to ADAS1000 datasheet (Table 55, 56):
        //  * Lead II = LL – RA → typically shows up in word 2
        //  * RESP output (impedance signal) often appears around word 4–6
        //  * Since we’re using 16-bit words, index them directly
        //  * You can adjust the index after confirming from actual data logs
        //  */

        // int16_t leadII_raw = rxBuf[2];  // Lead II (LL - RA)
        // int16_t resp_raw   = rxBuf[4];  // RESP signal

        // // Optionally sign-extend to 32-bit if needed
        // int32_t leadII = (int32_t)((int16_t)leadII_raw);
        // int32_t resp   = (int32_t)((int16_t)resp_raw);

        // // Format a debug UART message
        // char uartMsg[64];
        // snprintf(uartMsg, sizeof(uartMsg), "LeadII: %" PRId32 "\tRESP: %" PRId32 "\r\n", leadII, resp);
        // logOverUART(uartMsg);

        // // =============================================
        // // END: Lead II + RESP debug
        // // =============================================

        // // (Optional) Print raw SPI frame for inspection — helps confirm location of data
        // printf("Extended Frame: ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%04X ", rxBuf[i]);
        // }
        // printf("\n");

        Task_sleep(g_taskSleepDuration);
    }
}

void afe_spiInit(void){
    SPI_Params spiParams;
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL1_PHA1; // Mode 3 (CPOL=1, CPHA=1)
    spiParams.dataSize = 16;
    spiParams.bitRate = 1000000; // 1 MHz safe default

    g_afeSpiHandle = SPI_open(CONFIG_AFE_SPI, &spiParams);
}

void afe_writeRegister(uint8_t regAddr, uint16_t data){
    uint16_t command = ((regAddr & 0x1F) << 11) | (0 << 10) | (data & 0x3FF); // write op
    SPI_Transaction spiTransaction;
    spiTransaction.count = 2;
    spiTransaction.txBuf = &command;
    spiTransaction.rxBuf = NULL;

    SPI_transfer(g_afeSpiHandle, &spiTransaction);
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