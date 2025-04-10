#ifndef MCU_H
#define MCU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// === Core HAL Functions ===

void MCU_Init(void);

void MCU_Delay_20ms(void);
void MCU_Delay_500ms(void);

void MCU_CSlow(void);
void MCU_CShigh(void);

void MCU_PDlow(void);
void MCU_PDhigh(void);

uint8_t MCU_SPIReadWrite8(uint8_t data);

// SPI
void MCU_SPIWrite8(uint8_t data);
void MCU_SPIWrite16(uint16_t data);
void MCU_SPIWrite24(uint32_t data);
void MCU_SPIWrite32(uint32_t data);
uint8_t MCU_SPIRead8(void);
uint16_t MCU_SPIRead16(void);
uint32_t MCU_SPIRead32(void);

// Byte swap helpers
uint16_t MCU_htobe16(uint16_t x);
uint16_t MCU_htole16(uint16_t x);
uint32_t MCU_htobe32(uint32_t x);
uint32_t MCU_htole32(uint32_t x);
uint16_t MCU_le16toh(uint16_t x);
uint32_t MCU_le32toh(uint32_t x);

#ifdef __cplusplus
}
#endif

#endif // MCU_H
