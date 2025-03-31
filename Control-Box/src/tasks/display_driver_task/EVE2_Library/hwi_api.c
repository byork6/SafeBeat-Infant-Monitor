#include "../../../common/common.h"

// Globarl Vars
SPI_Handle g_spiHandle;

void HAL_SPI_Enable(void){
    GPIO_write(CONFIG_DISPLAY_CS, 0);
}

void HAL_SPI_Disable(void){
    GPIO_write(CONFIG_DISPLAY_CS, 1);
}

uint8_t HAL_SPI_Write(uint8_t data){
    SPI_Transaction transaction;
    uint8_t rxBuf = 0;

    transaction.count = 1;
    transaction.txBuf = &data;
    transaction.rxBuf = &rxBuf;

    if (SPI_transfer(g_spiHandle, &transaction)) {
        return rxBuf;
    } else {
        return 0xFF;  // This return statement shows that there is a failure
    }
}

void HAL_SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.count = Length;
    transaction.txBuf = Buffer;
    transaction.rxBuf = NULL;

    SPI_transfer(g_spiHandle, &transaction);
}

void HAL_SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;
    uint8_t dummyTx = 0x00;

    transaction.count = Length;
    transaction.txBuf = &dummyTx;
    transaction.rxBuf = Buffer;

    SPI_transfer(g_spiHandle, &transaction);
}

void HAL_Delay(uint32_t milliSeconds){
    Task_sleep(DEFAULT_TASK_SLEEP_DURATION);
}

void HAL_Eve_Reset_HW(void){
    // Optional --- Toggles reset GPIO pin if one is being used.
}

void HAL_Close(void){
    SPI_close(g_spiHandle);
}