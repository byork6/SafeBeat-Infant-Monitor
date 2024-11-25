#pragma once

/////// SDK inlcusions ///////
/* For usleep() */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* TI Driver Header files */
// #include <ti/drivers/GPIO.h>
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
#include <ti/drivers/SPI.h>
// #include <ti/drivers/Display.h>
#include <ti/display/Display.h>
// #include <ti/drivers/NVS.h> (Non-Volatile Storage)
#include <ti/drivers/NVS.h>

/* Driver configuration */
#include "ti_drivers_config.h"


/////// CUSTOM INCLUSIONS ///////
// Project Header Files
#include "functions.h"
#include "test_functions.h"

// Define GPIO Macros
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
#define LOOP_DELAY_US (250000)                                                  // 0.25 seconds