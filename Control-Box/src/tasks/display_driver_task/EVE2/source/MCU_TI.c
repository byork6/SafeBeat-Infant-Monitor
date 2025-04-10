// MCU_TI.c - Bridgetek BRT_AN_025 HAL for TI CC2651P3 LaunchPad
#include "../../common/common.h"

// Direct GPIO pin usage (not via SysConfig alias)
#define DISPLAY_PD_PIN   13
#define DISPLAY_CS_PIN   23  // Using GPIO pin 23 for Chip Select

// === Platform Entry Point ===

void MCU_Init(void) {
    GPIO_write(DISPLAY_CS_PIN, 1);
    GPIO_write(DISPLAY_PD_PIN, 1);
}

void MCU_Setup(void) {
    // Any post-reset logic, if needed
}

void MCU_CSlow(void) {
    GPIO_write(DISPLAY_CS_PIN, 0);
}

void MCU_CShigh(void) {
    GPIO_write(DISPLAY_CS_PIN, 1);
}

void MCU_PDlow(void) {
    GPIO_write(DISPLAY_PD_PIN, 0);
}

void MCU_PDhigh(void) {
    GPIO_write(DISPLAY_PD_PIN, 1);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length) {
    SPI_Transaction t = {
        .count = length,
        .txBuf = DataToWrite,
        .rxBuf = NULL
    };
    SPI_transfer(displaySpiHandle, &t);
}

void MCU_SPIWrite8(uint8_t data) {
    MCU_SPIWrite(&data, 1);
}

void MCU_SPIWrite16(uint16_t data) {
    uint8_t buf[2] = { data >> 8, data & 0xFF };
    MCU_SPIWrite(buf, 2);
}

void MCU_SPIWrite24(uint32_t data) {
    uint8_t buf[3] = {
        (data >> 16) & 0xFF,
        (data >> 8) & 0xFF,
        data & 0xFF
    };
    MCU_SPIWrite(buf, 3);
}

void MCU_SPIWrite32(uint32_t data) {
    uint8_t buf[4] = {
        (data >> 24) & 0xFF,
        (data >> 16) & 0xFF,
        (data >> 8) & 0xFF,
        data & 0xFF
    };
    MCU_SPIWrite(buf, 4);
}

uint8_t MCU_SPIRead8(void) {
    uint8_t tx = 0x00, rx;
    SPI_Transaction t = {
        .count = 1,
        .txBuf = &tx,
        .rxBuf = &rx
    };
    SPI_transfer(displaySpiHandle, &t);
    return rx;
}

uint16_t MCU_SPIRead16(void) {
    uint8_t tx[2] = { 0x00, 0x00 }, rx[2];
    SPI_Transaction t = {
        .count = 2,
        .txBuf = tx,
        .rxBuf = rx
    };
    SPI_transfer(displaySpiHandle, &t);
    return (rx[0] << 8) | rx[1];
}

uint32_t MCU_SPIRead24(void) {
    uint8_t tx[3] = { 0x00, 0x00, 0x00 }, rx[3];
    SPI_Transaction t = {
        .count = 3,
        .txBuf = tx,
        .rxBuf = rx
    };
    SPI_transfer(displaySpiHandle, &t);
    return (rx[0] << 16) | (rx[1] << 8) | rx[2];
}

uint32_t MCU_SPIRead32(void) {
    uint8_t tx[4] = { 0x00, 0x00, 0x00, 0x00 }, rx[4];
    SPI_Transaction t = {
        .count = 4,
        .txBuf = tx,
        .rxBuf = rx
    };
    SPI_transfer(displaySpiHandle, &t);
    return (rx[0] << 24) | (rx[1] << 16) | (rx[2] << 8) | rx[3];
}

void MCU_Delay_20ms(void) {
    usleep(20000);
}

void MCU_Delay_500ms(void) {
    usleep(500000);
}

// Endianness conversion (assumes CC2651P3 is little-endian)

uint16_t MCU_htobe16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

uint32_t MCU_htobe32(uint32_t x) {
    return ((x >> 24) & 0x000000FF) |
           ((x >> 8)  & 0x0000FF00) |
           ((x << 8)  & 0x00FF0000) |
           ((x << 24) & 0xFF000000);
}

uint16_t MCU_htole16(uint16_t x) { return x; }
uint32_t MCU_htole32(uint32_t x) { return x; }
uint16_t MCU_be16toh(uint16_t x) { return MCU_htobe16(x); }
uint32_t MCU_be32toh(uint32_t x) { return MCU_htobe32(x); }
uint16_t MCU_le16toh(uint16_t x) { return x; }
uint32_t MCU_le32toh(uint32_t x) { return x; }