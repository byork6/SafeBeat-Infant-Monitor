#include "../../../common/common.h"

void HAL_SPI_Enable(void){
    GPIO_write(CONFIG_SD_SPI_CS, 1);
    GPIO_write(CONFIG_DISPLAY_CS, 0);
}

void HAL_SPI_Disable(void){
    GPIO_write(CONFIG_SD_SPI_CS, 0);
    GPIO_write(CONFIG_DISPLAY_CS, 1);
}

uint8_t HAL_SPI_Write(uint8_t data){
    SPI_Transaction transaction;
    uint8_t rxBuf = 0;

    transaction.count = 1;
    transaction.txBuf = &data;
    transaction.rxBuf = &rxBuf;

    if (SPI_transfer(g_spiDisplayHandle, &transaction)) {
        // printf("HAL_SPI_Write: Returned rxBuf 0x%02X\n", rxBuf);
        return rxBuf;
    } else {
        printf("HAL_SPI_Write Failed!\n");
        return 0xFF;
    }
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
    Task_sleep(MS_TO_TICKS(milliSeconds));
}

void HAL_Eve_Reset_HW(void){
    GPIO_write(CONFIG_DISPLAY_SPI_PD, 0);  // Enter reset
    Task_sleep(MS_TO_TICKS(20));            // 20 ms
    
    GPIO_write(CONFIG_DISPLAY_SPI_PD, 1);  // Exit reset
    Task_sleep(MS_TO_TICKS(300));          // Wait 300 ms
}

void HAL_Close(void){
    SPI_close(g_spiDisplayHandle);
}