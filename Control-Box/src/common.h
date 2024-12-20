#pragma once

// Built-ins
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/////// SDK inlcusions ///////
// TI Driver Header files
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
// #include <ti/drivers/NVS.h> (Non-Volatile Storage)
#include <ti/drivers/NVS.h>
// #include <ti/ble5stack_flash/inc/bcomdef.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// Driver configuration
#include "ti_drivers_config.h"
// For RTOS BIOS execution
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/////// CUSTOM INCLUSIONS ///////
// Custom Header Files
#include "config_functions.h"

// Cusotom Macros
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
#define DELAY_DURATION_US ((uint32_t)(250000))                                  // 0.25 seconds --- 250,000 us
#define DELAY_US(us) ((us) / Clock_tickPeriod)                                  // Macro to convert microseconds to ticks
