#pragma once

// Built-in library inclusions
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
// Added for microSD write
#include <file.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


/* POSIX Header files */
#include <pthread.h>
#include <third_party/fatfs/ffcio.h>

/////// SDK HEADER CONTENT ///////
// SDK Driver Header files
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/rf/RF.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
// #include <ti/drivers/NVS.h>
#include <ti/drivers/SD.h>
#include <ti/drivers/SDFatFS.h>
#include <time.h>
// #include <ti/ble5stack_flash/inc/bcomdef.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// For TI-RTOS7 BIOS execution
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Hwi.h>


// Driver configuration
#include "ti_drivers_config.h"

/////// CUSTOM HEADER CONTENT ///////
// Startup config
#include "config_functions.h"

// Custom Macros
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
#define DELAY_DURATION_US ((uint32_t)(250000))                                  // 0.25 seconds --- 250,000 us
#define DELAY_US(us) ((us) / Clock_tickPeriod)                                  // Macro to convert microseconds to ticks
extern char fatfsPrefix[];


// Custom Function Prototypes
////////// DOC STRING TEMPLATE //////////
/**
* @brief - Short description goes here.
*
* Long description goes here (optional).
*
* @param a - First param (optional).
* @param b - Second param (optional).
* @return  - What is returned goes here (optional).
*/
/////////////////////////////////////////

/**
* @brief - Test code that toggles a GPIO pin every 1 second.
*
* The function sets a single GPIO pin to output and toggles it
* with a one second delay. This is used for testing the functionality of
* a digital output pin only and should not be used in the final project. 
* The input parameter is chosen based off of the gpioPinConfigs
* list inside ti_drivers_config.c.
*
* @param pin_config_index - The index of the GPIO pin to be tested. Valid inputs = 5-30
*/
void testGpio(uint32_t pin_config_index);

/**
* @brief Print the value of a variable of various types to the CIO with an optional name.
*
* This function prints the value of a variable based on its type.
* Supported types include integer, float, character, and string.
* The function uses a `void*` pointer to handle different types dynamically.
* Additionally, the variable name can be provided for descriptive output; if
* no name is provided (`varName` is `NULL`), a default name ("Unnamed Variable") is used.
*
* @param varName - Optional name of the variable to print. Pass `NULL` to use the default name.
* @param var - Pointer to the variable to be printed. The actual type of the variable
*              must match the specified type parameter (`type`).
* @param type - A character specifying the type of the variable:
*               - 'd' for integers
*               - 'f' for floats
*               - 'c' for characters
*               - 's' for strings
*
* @note Ensure the correct type is passed to avoid undefined behavior.
*       For example, if the type is 'd', ensure `var` points to an integer.
* 
* @example Example usage:
*          int num = 42;
*          printVar("num", &num, 'd');       // Prints: Variable "num" value: 42
*          printVar(NULL, &num, 'd');        // Prints: Variable "foo" value: 42
*/
void printVar(const char *varName, void *var, char type);


/**
* @brief Print a string to the CIO.
*
* @param str - Pointer to a string that will be printed to CIO.
* 
* @example Example usage:
*          printStr("Printed String");
*/
void printStr(const char *str);

/**
 * @brief Provides the current time in FAT file system format.
 *
 * This function converts the current system time into the FAT file system's 
 * time format, which is used to record timestamps for file creation, modification, 
 * and access within the FAT file system.
 *
 * FAT file system time format:
 * - Bits 31-25: Year offset from 1980 (e.g., 0 = 1980, 1 = 1981, etc.)
 * - Bits 24-21: Month (1 = January, 12 = December)
 * - Bits 20-16: Day of the month (1-31)
 * - Bits 15-11: Hour (0-23)
 * - Bits 10-5: Minute (0-59)
 * - Bits 4-0: Second divided by 2 (0-29)
 *
 * @return int32_t A 32-bit integer representing the current time in FAT file system format.
 *
 * @example Example usage:
 *     int32_t fatTime = fatfs_getFatTime();
 *     printf("FAT Time: 0x%08X\n", fatTime);
 *
 * @note This is required when using microSD_write_task.h
 */
int32_t fatfs_getFatTime(void);
