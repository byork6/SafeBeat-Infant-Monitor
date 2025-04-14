// MCU_TI.c - Bridgetek BRT_AN_025 HAL for TI CC2651P3 LaunchPad
//#include "../../common/common.h"
#include "../../../../common/common.h"
#include <string.h>

// Direct GPIO pin usage (not via SysConfig alias)
#define DISPLAY_PD_PIN   13
#define DISPLAY_CS_PIN   23  // Using GPIO pin 23 for Chip Select

// === Platform Entry Point ===

/*void MCU_Init(void) {
    GPIO_write(DISPLAY_CS_PIN, 1);
    GPIO_write(DISPLAY_PD_PIN, 1);
}*/
void MCU_Init(void) {
    MCU_CShigh();        // make sure CS starts HIGH
    MCU_PDlow();         // drive PD low
    usleep(20000);       // 20 ms reset delay
    MCU_PDhigh();        // bring PD high
    usleep(20000);       // 20 ms for wake-up

    // Send dummy clocks (3 bytes of 0x00) after PD HIGH
    uint8_t dummy[3] = { 0x00, 0x00, 0x00 };
    SPI_Transaction t = {
        .count = 3,
        .txBuf = dummy,
        .rxBuf = NULL
    };
    SPI_transfer(displaySpiHandle, &t);
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
    printf("[PD] -> LOW\n");
    GPIO_write(DISPLAY_PD_PIN, 0);
}

void MCU_PDhigh(void) {
    printf("[PD] -> HIGH\n");
    GPIO_write(DISPLAY_PD_PIN, 1);
}

void MCU_SPIWrite(const uint8_t *DataToWrite, uint32_t length) {
    SPI_Transaction t;
    memset(&t, 0, sizeof(SPI_Transaction));

    t.count = length;
    t.txBuf = (void *)DataToWrite;  // cast to fix warning
    t.rxBuf = NULL;

    bool ok = SPI_transfer(displaySpiHandle, &t);
    if (!ok) {
        printf("[SPI] SPI_transfer failed!\n");
    }
}

/*void MCU_SPIWrite8(uint8_t data) {
    //MCU_SPIWrite(&data, 1);
    uint8_t rx;
    SPI_Transaction t = {
        .count = 1,
        .txBuf = &data,
        .rxBuf = &rx
    };
    bool ok = SPI_transfer(displaySpiHandle, &t);
    if (!ok) {
        printf("[MCU_SPIWrite8] Transfer failed\n");
    } else {
        printf("[MCU_SPIWrite8] Sent 0x%02X, got 0x%02X\n", data, rx);
    }
}*/
void MCU_SPIWrite8(uint8_t data) {
    uint8_t rx;
    SPI_Transaction t = {
        .count = 1,
        .txBuf = &data,
        .rxBuf = &rx
    };

    MCU_CSlow();  // assert CS before transaction
    bool ok = SPI_transfer(displaySpiHandle, &t);
    MCU_CShigh(); // release CS after transaction

    if (!ok) {
        printf("[MCU_SPIWrite8] Transfer failed\n");
    } else {
        printf("[MCU_SPIWrite8] Sent 0x%02X, got 0x%02X\n", data, rx);
    }
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

/*uint8_t MCU_SPIRead8(void) {
    uint8_t tx = 0x00, rx;
    SPI_Transaction t = {
        .count = 1,
        .txBuf = &tx,
        .rxBuf = &rx
    };
    SPI_transfer(displaySpiHandle, &t);
    return rx;
}*/
uint8_t MCU_SPIRead8(void) {
    uint8_t tx = 0x00, rx;
    SPI_Transaction t = {
        .count = 1,
        .txBuf = &tx,
        .rxBuf = &rx
    };

    MCU_CSlow();
    SPI_transfer(displaySpiHandle, &t);
    MCU_CShigh();

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