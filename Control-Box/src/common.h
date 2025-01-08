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
// TI Driver Header files
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/rf/RF.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>
// #include <ti/drivers/NVS.h>
#include <ti/drivers/SD.h>
#include <ti/drivers/SDFatFS.h>
// #include <ti/ble5stack_flash/inc/bcomdef.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

// For TI-RTOS7 BIOS execution
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

// Driver configuration
#include "ti_drivers_config.h"

/////// CUSTOM HEADER CONTENT ///////
// Startup config
#include "config_functions.h"
#include "test_functions.h"

// Custom Macros
#define DRIVE_GPIO_HIGH (1)
#define DRIVE_GPIO_LOW (0)
#define GPIO_SET_OUT_AND_DRIVE_LOW (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW)
#define GPIO_SET_OUT_AND_DRIVE_HIGH (GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH)
<<<<<<< HEAD
#define DELAY_DURATION_US ((uint32_t)(1000000))                                  // 0.25 seconds --- 250,000 us
#define DELAY_US(us) ((us) / Clock_tickPeriod)                                   // Macro to convert microseconds to ticks --- Clock_tickPeriod = 10
=======
#define DELAY_DURATION_US ((uint32_t)(250000))                                  // 0.25 seconds --- 250,000 us
#define DELAY_US(us) ((us) / Clock_tickPeriod)                                  // Macro to convert microseconds to ticks

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
>>>>>>> main
