// MCU_TI.c - Bridgetek BRT_AN_025 HAL for TI CC2651P3 LaunchPad

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Board.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/clock/Clock.h>

// Direct GPIO pin usage (not via SysConfig alias)
#define DISPLAY_PD_PIN   CONFIG_DISPLAY_SPI_PD
#define DISPLAY_CS_PIN   23  // Using GPIO pin 23 for Chip Select

extern SPI_Handle displaySpiHandle;

// === Platform Entry Point ===
void MCU_Init(void) {
    // Set GPIO to known states
    GPIO_write(DISPLAY_PD_PIN, 1);  // Not in reset
    GPIO_write(DISPLAY_CS_PIN, 1);  // CS inactive
}

void MCU_Delay_20ms(void) {
    Task_sleep(20 * (1000 / Clock_tickPeriod));
}

void MCU_Delay_500ms(void) {
    Task_sleep(500 * (1000 / Clock_tickPeriod));
}

// === GPIO Control ===
void MCU_CSlow(void) {
    GPIO_write(11, GPIO_SET_OUT_AND_DRIVE_LOW);   // Optional: tied signal low
    GPIO_write(DISPLAY_CS_PIN, GPIO_SET_OUT_AND_DRIVE_LOW);  // Select CS
}

void MCU_CShigh(void) {
    GPIO_write(11, GPIO_SET_OUT_AND_DRIVE_HIGH);  // Optional: tied signal high
    GPIO_write(DISPLAY_CS_PIN, GPIO_SET_OUT_AND_DRIVE_HIGH); // Deselect CS
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