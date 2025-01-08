#pragma once

#include <stdint.h>


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
*               - 'u' for unsigned integers
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
* @brief Print a string
*
* This function prints the value of a variable based on its type.
* Supported types include integer, float, character, and string.
* The function uses a `void*` pointer to handle different types dynamically.
* Additionally, the variable name can be provided for descriptive output; if
* no name is provided (`varName` is `NULL`), a default name ("Unnamed Variable") is used.
*
* @param str - Pointer to the string to be printed
*
*/
void printStr(const char *str);
