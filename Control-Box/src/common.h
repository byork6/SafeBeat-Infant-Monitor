#pragma once

// Built-ins
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/////// SDK inlcusions ///////
// TI Driver Header files
#include <ti/drivers/GPIO.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
// #include <ti/drivers/NVS.h> (Non-Volatile Storage)
#include <ti/drivers/NVS.h>
// #include <ti/ble5stack_flash/inc/bcomdef.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// For rf_packet_rx_task.c
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)
#include <ti_radio_config.h>

// For TIRTOS7 BIOS execution
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

// Driver configuration
#include "ti_drivers_config.h"

/////// CUSTOM INCLUSIONS ///////
// Startup config
#include "config_functions.h"
// RF packet receive task
#include "tasks/rf_packet_rx_task/rf_packet_rx_task.h"
#include "tasks/rf_packet_rx_task/rf_queue.h"

// Custom Macros
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
#define DELAY_DURATION_US ((uint32_t)(250000))                                  // 0.25 seconds --- 250,000 us
#define DELAY_US(us) ((us) / Clock_tickPeriod)                                  // Macro to convert microseconds to ticks
