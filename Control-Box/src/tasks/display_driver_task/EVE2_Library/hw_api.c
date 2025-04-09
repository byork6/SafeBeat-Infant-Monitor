#include "../../../common/common.h"
#include "display_driver.h"

void HAL_SPI_Enable(void){
    GPIO_write(11, GPIO_SET_OUT_AND_DRIVE_HIGH);
    GPIO_write(23, GPIO_SET_OUT_AND_DRIVE_LOW);
    printf("Display SPI Enabled\n");
    // printf("Sleeping for 1 second...\n");
    usleep(1000000);
}

void HAL_SPI_Disable(void){
    GPIO_write(11, GPIO_SET_OUT_AND_DRIVE_LOW);
    GPIO_write(23, GPIO_SET_OUT_AND_DRIVE_HIGH);
    printf("Display SPI Disabled\n");
    // printf("Sleeping for 1 second...\n");
    usleep(1000000);
}

uint8_t HAL_SPI_Write(uint8_t data){
    printf("Writing 0x%02X...\n", data);

    displayTransmitBuffer[0] = data;

    // Fill in transmitBuffer
    g_spiDisplayTransaction.count = MSGSIZE;
    g_spiDisplayTransaction.txBuf = (void *)displayTransmitBuffer;
    g_spiDisplayTransaction.rxBuf = (void *)displayReceiveBuffer;

    displayTransferOK = SPI_transfer(g_spiDisplayHandle, &g_spiDisplayTransaction);
    if (!displayTransferOK) {
        printf("Transfer NOT OK");
        printf("Spinning...");
        // Error in SPI or transfer already in progress.
        while (1);
    }
    printf("txBuf content: 0x%02X\n", ((uint8_t *)g_spiDisplayTransaction.txBuf)[0]);
    return 0;
}

void HAL_SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.count = Length;
    transaction.txBuf = Buffer;
    transaction.rxBuf = NULL;

    SPI_transfer(g_spiDisplayHandle, &transaction);
}

void HAL_SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;
    uint8_t dummyTx = 0x00;

    transaction.count = Length;
    transaction.txBuf = &dummyTx;
    transaction.rxBuf = Buffer;

    SPI_transfer(g_spiDisplayHandle, &transaction);
}

void HAL_Delay(uint32_t milliSeconds){
    usleep(1000 * (milliSeconds));
}

void HAL_Eve_Reset_HW(void){
    printf("Drive PD LOW\n");
    GPIO_write(CONFIG_DISPLAY_SPI_PD, 0);  // Enter reset
    usleep(20000);            // 20 ms
    
    printf("Drive PD HIGH\n");
    GPIO_write(CONFIG_DISPLAY_SPI_PD, 1);  // Exit reset
    usleep(300000);          // Wait 300 ms
}

void HAL_Close(void){
    SPI_close(g_spiDisplayHandle);
}