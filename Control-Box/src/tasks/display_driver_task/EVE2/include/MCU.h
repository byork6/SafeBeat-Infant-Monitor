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

#ifdef __cplusplus
}
#endif

#endif // MCU_H
