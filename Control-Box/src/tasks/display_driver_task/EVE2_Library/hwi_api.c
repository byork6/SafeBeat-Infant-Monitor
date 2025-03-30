#include "../../../common/common.h"

// TODO: Write the HAL api functions based off the EVE examples
SPI_Handle gSpiHandle;
/* HAL_SPI_Enable() is to drive the CS Pin to the eve HIGH */
void HAL_SPI_Enable(void){
    GPIO_write(CONFIG_DISPLAY_CS, 0);
}

/* HAL_SPI_Disable() is to drive the CS Pin to the eve LOW */
void HAL_SPI_Disable(void){
    GPIO_write(CONFIG_DISPLAY_CS, 1);
}

/* HAL_SPI_Write(uint8_t data) does a single byte SPI write transfer */
uint8_t HAL_SPI_Write(uint8_t data){
    SPI_Transaction transaction;
    uint8_t rxBuf = 0;

    transaction.count = 1;
    transaction.txBuf = &data;
    transaction.rxBuf = &rxBuf;

    if (SPI_transfer(gSpiHandle, &transaction)) {
        return rxBuf;
    } else {
        return 0xFF;  // This return statement shows that there is a failure
    }
}

/* HAL_SPI_WriteBuffer does a buffer based SPI Write transfer */
void HAL_SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;

    transaction.count = Length;
    transaction.txBuf = Buffer;
    transaction.rxBuf = NULL;

    SPI_transfer(gSpiHandle, &transaction);
}

/* HAL_SPI_WriteBuffer does a buffer based SPI Read transfer */
void HAL_SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length){
    SPI_Transaction transaction;
    uint8_t dummyTx = 0x00;

    transaction.count = Length;
    transaction.txBuf = &dummyTx;  // Repeated dummy writes
    transaction.rxBuf = Buffer;

    SPI_transfer(gSpiHandle, &transaction);
}

/* Stall the cpu for X milliseconds */
void HAL_Delay(uint32_t milliSeconds){
    Task_sleep((milliSeconds * 1000) / Clock_tickPeriod);
}

/* Gives an opertunity to reset the EVE hardware */
void HAL_Eve_Reset_HW(void){
    
}

/* Cleans up and resources allocated */
void HAL_Close(void){
    SPI_close(gSpiHandle);
}