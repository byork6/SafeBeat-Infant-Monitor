// MCU_TI.c - Bridgetek BRT_AN_025 HAL for TI CC2651P3 LaunchPad
#include "../../common/common.h"

// Direct GPIO pin usage (not via SysConfig alias)
#define DISPLAY_PD_PIN   13
#define DISPLAY_CS_PIN   23  // Using GPIO pin 23 for Chip Select

// === Platform Entry Point ===
void MCU_Init(void) {
    // Set GPIO to known states
    GPIO_write(DISPLAY_PD_PIN, 1);  // Not in reset
    GPIO_write(DISPLAY_CS_PIN, 1);  // CS inactive
}

void MCU_Delay_20ms(void) {
    Task_sleep(MS_TO_TICKS(200));
}

void MCU_Delay_500ms(void) {
    Task_sleep(MS_TO_TICKS(500));
}

// === GPIO Control ===
void MCU_CSlow(void) {
    GPIO_write(11, 1);   // Optional: tied signal low
    GPIO_write(DISPLAY_CS_PIN, 0);  // Select CS
}

void MCU_CShigh(void) {
    GPIO_write(11, 0);  // Optional: tied signal high
    GPIO_write(DISPLAY_CS_PIN, 1); // Deselect CS
}

void MCU_PDlow(void) {
    GPIO_write(DISPLAY_PD_PIN, 0);
}

void MCU_PDhigh(void) {
    GPIO_write(DISPLAY_PD_PIN, 1);
}

// === SPI Byte Transfer ===
uint8_t MCU_SPIReadWrite8(uint8_t data) {
    uint8_t rx;
    SPI_Transaction transaction;
    transaction.count = 1;
    transaction.txBuf = &data;
    transaction.rxBuf = &rx;
    transaction.arg = NULL;
    transaction.status = 0;
    transaction.nextPtr = NULL;
    SPI_transfer(displaySpiHandle, &transaction);
    return rx;
}

void MCU_SPIWrite8(uint8_t data) {
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 1;
    transaction.txBuf = &data;
    transaction.rxBuf = NULL;
    SPI_transfer(displaySpiHandle, &transaction);
}

void MCU_SPIWrite16(uint16_t data) {
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 2;
    transaction.txBuf = buf;
    transaction.rxBuf = NULL;
    SPI_transfer(displaySpiHandle, &transaction);
}

void MCU_SPIWrite24(uint32_t data) {
    uint8_t buf[3] = {
        (data >> 16) & 0xFF,
        (data >> 8) & 0xFF,
        data & 0xFF
    };
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 3;
    transaction.txBuf = buf;
    transaction.rxBuf = NULL;
    SPI_transfer(displaySpiHandle, &transaction);
}

void MCU_SPIWrite32(uint32_t data) {
    uint8_t buf[4] = {
        (data >> 24) & 0xFF,
        (data >> 16) & 0xFF,
        (data >> 8) & 0xFF,
        data & 0xFF
    };
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 4;
    transaction.txBuf = buf;
    transaction.rxBuf = NULL;
    SPI_transfer(displaySpiHandle, &transaction);
}

// === SPI Read ===

uint8_t MCU_SPIRead8(void) {
    uint8_t tx = 0x00;
    uint8_t rx;
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 1;
    transaction.txBuf = &tx;
    transaction.rxBuf = &rx;
    SPI_transfer(displaySpiHandle, &transaction);
    return rx;
}

uint16_t MCU_SPIRead16(void) {
    uint8_t tx[2] = { 0x00, 0x00 };
    uint8_t rx[2];
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 2;
    transaction.txBuf = tx;
    transaction.rxBuf = rx;
    SPI_transfer(displaySpiHandle, &transaction);
    return (rx[0] << 8) | rx[1];
}

uint32_t MCU_SPIRead32(void) {
    uint8_t tx[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint8_t rx[4];
    SPI_Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.count = 4;
    transaction.txBuf = tx;
    transaction.rxBuf = rx;
    SPI_transfer(displaySpiHandle, &transaction);
    return (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | rx[3];
}

// === Endianness Conversion Helpers ===

uint16_t MCU_htobe16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

uint16_t MCU_htole16(uint16_t x) {
    return x; // TI CC2651P3 is little-endian
}

uint32_t MCU_htobe32(uint32_t x) {
    return ((x >> 24) & 0x000000FF) |
           ((x >> 8)  & 0x0000FF00) |
           ((x << 8)  & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}

uint32_t MCU_htole32(uint32_t x) {
    return x;
}

uint16_t MCU_le16toh(uint16_t x) {
    return x;
}

uint32_t MCU_le32toh(uint32_t x) {
    return x;
}